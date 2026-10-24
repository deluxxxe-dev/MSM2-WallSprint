#include "GameInterop/log_file.hpp"
#include <algorithm>
#include <cassert>
#include <cstdio>
#include <mutex>
#include <set>
#include <thread>
#include <vector>
static void write(HANDLE f, const char *text) {
  DWORD n{};
  assert(f != INVALID_HANDLE_VALUE);
  assert(WriteFile(f, text, DWORD(std::char_traits<char>::length(text)), &n,
                   nullptr));
}
static std::string read(const std::wstring &p) {
  auto f =
      CreateFileW(p.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
                  nullptr, OPEN_EXISTING, 0, nullptr);
  assert(f != INVALID_HANDLE_VALUE);
  char data[256]{};
  DWORD n{};
  assert(ReadFile(f, data, sizeof(data), &n, nullptr));
  CloseHandle(f);
  return {data, n};
}
int main() {
  SYSTEMTIME local{2026, 1, 0, 2, 3, 4, 5, 0};
  assert(ws::logging::filename(local) ==
         L"MSM2-WS-v0.1c20-20260102-030405.log");
  assert(ws::logging::filename(local, 1) ==
         L"MSM2-WS-v0.1c20-20260102-030405-01.log");
  assert(ws::logging::filename(local, 12) ==
         L"MSM2-WS-v0.1c20-20260102-030405-12.log");
  assert(
      ws::logging::rootDirectory(L"C:\\Games\\Spider Man 2\\Spider-Man2.exe") ==
      L"C:\\Games\\Spider Man 2");
  assert(ws::logging::rootDirectory(L"game.exe").empty());
  puts("PASS: workspace logger derives executable root, versioned nested "
       "project folder and "
       "zero-padded local SYSTEMTIME timestamp/suffix without hard-coded "
       "install path");
  wchar_t temp[MAX_PATH]{}, unique[MAX_PATH]{};
  assert(GetTempPathW(MAX_PATH, temp));
  assert(GetTempFileNameW(temp, L"wsl", 0, unique));
  assert(DeleteFileW(unique));
  assert(CreateDirectoryW(unique, nullptr));
  const std::wstring base = unique;
  const std::wstring root = base + L"\\Game \u30c6\u30b9\u30c8 \u58c1 space";
  assert(CreateDirectoryW(root.c_str(), nullptr));
  const std::wstring exe = root + L"\\Spider-Man2.exe",
                     shared = root + L"\\logs";
  assert(CreateDirectoryW(shared.c_str(), nullptr));
  auto sentinel =
      CreateFileW((shared + L"\\WallSprint.log").c_str(), GENERIC_WRITE,
                  FILE_SHARE_READ, nullptr, CREATE_NEW, 0, nullptr);
  write(sentinel, "foreign_shared_log");
  CloseHandle(sentinel);
  ws::logging::Session first, second, third;
  auto f = first.openAt(exe, local);
  assert(f != INVALID_HANDLE_VALUE);
  write(f, "A_session\n");
  assert(first.path() ==
         root + L"\\Mods "
                L"Library\\Mods-Developing\\MSM2-WS\\logs\\MSM2-WS-v0.1c20-"
                L"20260102-030405.log");
  assert(first.openAt(exe, local) == INVALID_HANDLE_VALUE &&
         first.error() == ERROR_SHARING_VIOLATION);
  CloseHandle(f);
  auto original = first.path();
  local.wSecond = 6;
  f = first.openAt(exe, local);
  assert(first.path() == original);
  write(f, "E_session\n");
  CloseHandle(f);
  local.wSecond = 5;
  f = second.openAt(exe, local);
  write(f, "second");
  CloseHandle(f);
  f = third.openAt(exe, local);
  write(f, "third");
  CloseHandle(f);
  assert(second.path().find(L"-01.log") != std::wstring::npos &&
         third.path().find(L"-02.log") != std::wstring::npos);
  assert(read(first.path()) == "A_session\nE_session\n" &&
         read(second.path()) == "second");
  std::mutex lock;
  std::set<std::wstring> paths;
  std::vector<std::thread> threads;
  for (unsigned i = 0; i < 4; ++i)
    threads.emplace_back([&] {
      ws::logging::Session run;
      auto h = run.openAt(exe, local);
      assert(h != INVALID_HANDLE_VALUE);
      write(h, "concurrent");
      CloseHandle(h);
      std::lock_guard<std::mutex> guard(lock);
      assert(paths.insert(run.path()).second);
    });
  for (auto &thread : threads)
    thread.join();
  assert(paths.size() == 4 &&
         read(shared + L"\\WallSprint.log") == "foreign_shared_log");
  puts("PASS: workspace actual Unicode/space directory creation/atomic "
       "collision suffixes preserve "
       "existing and concurrent logs; A/E and reopen append to one fixed "
       "timestamped run file");
  ws::logging::Session exhausted;
  assert(exhausted.openAt(exe, local, 1) == INVALID_HANDLE_VALUE &&
         exhausted.error() == ERROR_FILE_EXISTS);
  assert(std::wstring(exhausted.stage()) == L"collision_limit");
  const auto blocked = root + L"\\blocked";
  assert(CreateDirectoryW(blocked.c_str(), nullptr));
  auto block = CreateFileW((blocked + L"\\Mods Library").c_str(), GENERIC_WRITE,
                           FILE_SHARE_READ, nullptr, CREATE_NEW, 0, nullptr);
  write(block, "directory_blocker");
  CloseHandle(block);
  ws::logging::Session failure;
  assert(failure.openAt(blocked + L"\\game.exe", local) ==
             INVALID_HANDLE_VALUE &&
         failure.error() == ERROR_DIRECTORY);
  assert(GetFileAttributesW((blocked + L"\\logs").c_str()) ==
             INVALID_FILE_ATTRIBUTES &&
         read(blocked + L"\\Mods Library") == "directory_blocker");
  ws::logging::Session missingRoot;
  assert(missingRoot.openAt(L"game.exe", local) == INVALID_HANDLE_VALUE &&
         missingRoot.error() == ERROR_INVALID_NAME);
  assert(read(shared + L"\\WallSprint.log") == "foreign_shared_log");
  puts("PASS: workspace directory/root/open/collision failures report explicit "
       "stage/error and never use or modify the shared logs path");
  for (const auto &p : paths)
    assert(DeleteFileW(p.c_str()));
  for (const auto &p :
       {first.path(), second.path(), third.path(), shared + L"\\WallSprint.log",
        blocked + L"\\Mods Library"})
    assert(DeleteFileW(p.c_str()));
  assert(GetFileAttributesW((root + L"\\log_wallsprint").c_str()) ==
         INVALID_FILE_ATTRIBUTES);
  assert(GetFileAttributesW((root + L"\\Mods_Developing").c_str()) ==
         INVALID_FILE_ATTRIBUTES);
  for (const auto part :
       {L"\\Mods Library\\Mods-Developing\\MSM2-WS\\logs",
        L"\\Mods Library\\Mods-Developing\\MSM2-WS",
        L"\\Mods Library\\Mods-Developing", L"\\Mods Library"})
    assert(RemoveDirectoryW((root + part).c_str()));
  assert(RemoveDirectoryW(shared.c_str()));
  assert(RemoveDirectoryW(blocked.c_str()));
  assert(RemoveDirectoryW(root.c_str()));
  assert(RemoveDirectoryW(base.c_str()));
}
