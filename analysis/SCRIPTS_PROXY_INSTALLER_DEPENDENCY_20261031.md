# scripts_proxy installer source versus winmm loader — 2026-10-31 JST

VERIFIED-STATIC (public Overstrike source): tag OS/v1.8.1, peeled commit
5829f3b045397ed19a1c3f282de504ba57197b7b; project Assembly/FileVersion1.8.1.0.
MetaInstallerBase.SetupScripts builds destination <game>/winmm.dll and, when
script support is enabled, calls File.Copy("scripts_proxy.dll", destination).
The source is process-CWD relative, not a coded absolute exe-adjacent path.
The actual owner exception requested <game>/Overstrike/scripts_proxy.dll.
This supports allowing that installer dependency, not a universal exe-directory
resolution claim or authentication of the owner's executable bytes.

Owner evidence (supplied task record): disabled script support produced no WS log;
enabled support failed on that missing source. Root proxy was COPIED beside the
manager, root copy retained; later owner log loaded v0.1c38. No separate successful
Install log exists; this does not prove r1 loaded, or real Update-WS succeeded.

workspace.game_preflight and Mod-Tools.environment_ready conflated both DLLs.
Now an optional existing Overstrike/scripts_proxy.dll must be a regular,
non-reparse file; absence remains allowed for compatibility with existing
preflight modes. It is an installer input, not the game loader. Existing required
root files remain required, misplaced Overstrike/winmm.dll remains refused,
including dangling links, and existing launcher quarantine remains unchanged.
No new source/root hash-equality requirement: installed source and root fixture
bytes may differ. No DLL auto-copy/download/delete or profile/settings changes.

Start/Open/Play share environment_ready. Start CMD itself only quarantines winmm
and clears existing root ReadOnly attributes; it does not delete scripts_proxy
and needed no code change. Root ReadOnly normalization is not proof the actual
installer source beside the manager is writable, or Install succeeded.

Updater.update calls workspace_paths/game_preflight before its git pull. Thus an
old installed checker cannot fetch its own correction. Use the guarded one-time
Git fetch/fast-forward in CODEX_RESULT; it preserves dirty/diverged branches and
then leaves normal validation in place. Installed Mod-Tools refresh uses the
existing source environment/Start-Overstrike.cmd entrypoint (KIT byte comparison
already detects changed engine even with unchanged tool version). No loader
relocation is necessary for the checker refresh.

All runtime/reversal/C52 gates unchanged. Wisdoms shared-environment references
were read only; durable correction is here and sync remains pending to keep this
patch bounded. Old blanket duplicate guidance is superseded by this distinction.
