# Windows-only owned fixture for recurring Overstrike winmm.dll ReadOnly repair.
$ErrorActionPreference = 'Stop'
if (-not $IsWindows -and $PSVersionTable.PSEdition -ne 'Desktop') {
    Write-Host 'SKIP: loader ReadOnly preflight requires Windows file attributes.'
    exit 0
}

$temp = Join-Path ([IO.Path]::GetTempPath()) ('MSM2-loader-preflight-' + [Guid]::NewGuid().ToString('N'))
$global:Launches = @()
function Get-Process { param([string]$Name, $ErrorAction) @() }
function Start-Process { param($FilePath, $WorkingDirectory, $ArgumentList) $global:Launches += [pscustomobject]@{File=$FilePath;Cwd=$WorkingDirectory;Args=$ArgumentList} }
try {
    $game = Join-Path $temp 'Game'
    New-Item -ItemType Directory -Path (Join-Path $game 'Overstrike') -Force | Out-Null
    [IO.File]::WriteAllBytes((Join-Path $game 'Spider-Man2.exe'), [byte[]](1,2,3))
    [IO.File]::WriteAllBytes((Join-Path $game 'Overstrike\Overstrike.exe'), [byte[]](4,5,6))
    [IO.File]::WriteAllBytes((Join-Path $game 'scripts_proxy.dll'), [byte[]](7,8,9))
    [IO.File]::WriteAllBytes((Join-Path $game 'winmm.dll'), [byte[]](10,11,12))

    $before = @{}
    foreach ($name in @('scripts_proxy.dll','winmm.dll')) {
        $path = Join-Path $game $name
        $before[$name] = (Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash
        $item = Get-Item -LiteralPath $path -Force
        [IO.File]::SetAttributes($path, $item.Attributes -bor [IO.FileAttributes]::ReadOnly)
        if (((Get-Item -LiteralPath $path -Force).Attributes -band [IO.FileAttributes]::ReadOnly) -eq 0) {
            throw "Fixture did not become ReadOnly: $name"
        }
    }

    $request = Join-Path $temp 'request.json'
    [IO.File]::WriteAllText($request, (@{action='manager';game=$game} | ConvertTo-Json), [Text.UTF8Encoding]::new($false))
    & (Join-Path $PSScriptRoot 'windows.ps1') -Request $request

    foreach ($name in @('scripts_proxy.dll','winmm.dll')) {
        $path = Join-Path $game $name
        if (((Get-Item -LiteralPath $path -Force).Attributes -band [IO.FileAttributes]::ReadOnly) -ne 0) {
            throw "ReadOnly remained after manager preflight: $name"
        }
        if ((Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash -ne $before[$name]) {
            throw "Loader bytes changed while clearing ReadOnly: $name"
        }
    }
    if ($global:Launches.Count -ne 1 -or $global:Launches[0].File -ne (Join-Path $game 'Overstrike\Overstrike.exe')) {
        throw 'Expected exactly one shared Overstrike launch request.'
    }
    Write-Host 'PASS: manager preflight clears only ReadOnly on scripts_proxy.dll/winmm.dll and preserves bytes.'
} finally {
    if (Test-Path -LiteralPath $temp) {
        foreach ($p in Get-ChildItem -LiteralPath $temp -Recurse -Force -File -ErrorAction SilentlyContinue) {
            try { [IO.File]::SetAttributes($p.FullName, [IO.FileAttributes]::Normal) } catch {}
        }
        Remove-Item -LiteralPath $temp -Recurse -Force -ErrorAction SilentlyContinue
    }
}
