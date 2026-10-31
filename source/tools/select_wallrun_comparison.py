"""Select only the installed WS r7 startup INI; never edits script archives/profile/source."""
from pathlib import Path
import argparse,hashlib,json,os,re,uuid,zipfile
import workspace as ws

KEY=rb'(?m)^(WALLSPRINT_NATIVE_SOURCE_OVERRIDE=)[01](\r?)$'
def select(game,mode,system=None):
    if mode not in ('ON','OFF','RESTORE'):raise ValueError('Invalid comparison mode')
    game=Path(game).absolute();system=system or ws.WindowsSystem();system.stop(game)
    with ws.operation_lock.hold(game,'comparison_config'):
        root=game/'Mods Library/Mods-Developing/MSM2-WS';ws.assert_layout(root,'MSM2-WS-v0.1c38r7.script')
        package=root/'MSM2-WS-v0.1c38r7.script';ini=game/'scripts/WallSprint.ini';dll=game/'scripts/WallSprint.dll';backup=root/'_archive/WallSprint-comparison-original.ini'
        for path in (package,ini,dll,backup):ws.operation_lock.safe(path)
        if not package.is_file() or package.stat().st_size>64*1024*1024 or not ini.is_file() or ini.stat().st_size>65536 or not dll.is_file() or dll.stat().st_size>64*1024*1024:raise ValueError('Installed WS file missing/size capped')
        package_digest=hashlib.sha256(package.read_bytes()).digest()
        with zipfile.ZipFile(package) as z:
            if len(z.infolist())!=3 or set(z.namelist())!={'info.json','WallSprint.dll','WallSprint.ini'} or any(i.file_size>64*1024*1024 for i in z.infolist()):raise ValueError('Invalid WS script shape')
            info=json.loads(z.read('info.json'));original=z.read('WallSprint.ini');binary=z.read('WallSprint.dll')
            if info.get('name')!='WallSprint' or info.get('version')!='0.1.63' or len(original)>65536 or len(re.findall(KEY,original))!=1:raise ValueError('Wrong WS version/config')
        if dll.read_bytes()!=binary:raise ValueError('Installed WS DLL differs from selected script; install current WS first')
        current=ini.read_bytes();on=re.sub(KEY,rb'\g<1>1\2',original);off=re.sub(KEY,rb'\g<1>0\2',original)
        if current not in (original,on,off):raise ValueError('Unrecognized installed config edits preserved')
        if backup.exists():
            if backup.read_bytes()!=original:raise ValueError('Existing comparison backup differs; preserved')
        else:
            if mode=='RESTORE':raise ValueError('No original comparison backup')
            if current!=original:raise ValueError('First selection requires original packaged INI')
            with backup.open('xb') as f:f.write(original)
        replacement=original if mode=='RESTORE' else on if mode=='ON' else off
        system.stop(game)
        if ini.read_bytes()!=current or dll.read_bytes()!=binary or hashlib.sha256(package.read_bytes()).digest()!=package_digest:raise ValueError('Installed identity/config changed; no replacement')
        temp=ini.with_name('.WallSprint-comparison-'+uuid.uuid4().hex+'.tmp')
        try:
            with temp.open('xb') as f:f.write(replacement);f.flush();os.fsync(f.fileno())
            os.replace(temp,ini)
        finally:
            if temp.exists():temp.unlink()
        print('WALLRUN_COMPARISON selected='+mode+' startup_restart_required=1 ini='+str(ini)+'; do not Install mods again between selection and launch')
        return replacement

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--game-root',type=Path,default=None);p.add_argument('--mode',choices=['ON','OFF','RESTORE'],required=True);a=p.parse_args()
    if a.game_root is None:
        repo=Path(__file__).resolve().parents[2]
        if repo.name!='_source' or repo.parent.name!='MSM2-WS':p.error('--game-root required outside managed _source layout')
        a.game_root=repo.parents[3]
    try:select(a.game_root,a.mode)
    except (ValueError,RuntimeError,OSError,zipfile.BadZipFile) as ex:p.exit(2,'WS COMPARISON REFUSED: '+str(ex)+'; existing unrelated files preserved\n')
if __name__=='__main__':main()
