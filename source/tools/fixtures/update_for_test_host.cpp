// TEST ONLY: owned Overstrike stand-in and game-root dependency, no real mod
// code.
#include <windows.h>
#ifndef WS_HELPER_DEPENDENCY
#include <cwchar>
int main() {
  wchar_t cwd[32768]{}, expected[32768]{}, report[32768]{};
  if (!GetCurrentDirectoryW(32768, cwd) ||
      !GetEnvironmentVariableW(L"MSM2_WS_GAME_ROOT", expected, 32768) ||
      _wcsicmp(cwd, expected) ||
      !GetEnvironmentVariableW(L"MSM2_WS_TEST_REPORT", report, 32768))
    return 10;
  auto dependency = LoadLibraryW(L"scripts_proxy.dll");
  if (!dependency)
    return 11;
  FreeLibrary(dependency);
  auto f = CreateFileW(report, GENERIC_WRITE, 0, nullptr, CREATE_NEW,
                       FILE_ATTRIBUTE_NORMAL, nullptr);
  if (f == INVALID_HANDLE_VALUE)
    return 12;
  constexpr char result[] = "GAME_ROOT_CWD_AND_DEPENDENCY_PASS";
  DWORD written{};
  bool ok = WriteFile(f, result, sizeof(result) - 1, &written, nullptr) &&
            written == sizeof(result) - 1;
  CloseHandle(f);
  return ok ? 0 : 13;
}
#else
BOOL WINAPI DllMain(HINSTANCE, DWORD, LPVOID) { return TRUE; }
#endif
