#pragma once
// Case-sensitive cross-compilation shim; original public headers use <Windows.h>.
// Use include_next so this shim also works from case-insensitive /mnt/c mounts,
// where <windows.h> can otherwise resolve back to this same file.
#include_next <windows.h>
