#pragma once
#include "version.hpp"
#include <cstdio>
#include <string>
#include <utility>
#include <windows.h>
namespace ws::logging {
inline std::wstring rootDirectory(const std::wstring &executable) {
  const auto pos = executable.find_last_of(L"\\/");
  return pos == std::wstring::npos ? std::wstring{} : executable.substr(0, pos);
}
inline std::wstring filename(const SYSTEMTIME &time, unsigned suffix = 0) {
  wchar_t text[160]{};
  const std::wstring version(ws::versionLabel,
                             ws::versionLabel + sizeof(ws::versionLabel) - 1);
  if (suffix)
    std::swprintf(text, 160, L"MSM2-WS-%ls-%04u%02u%02u-%02u%02u%02u-%02u.log",
                  version.c_str(), unsigned(time.wYear), unsigned(time.wMonth),
                  unsigned(time.wDay), unsigned(time.wHour),
                  unsigned(time.wMinute), unsigned(time.wSecond), suffix);
  else
    std::swprintf(text, 160, L"MSM2-WS-%ls-%04u%02u%02u-%02u%02u%02u.log",
                  version.c_str(), unsigned(time.wYear), unsigned(time.wMonth),
                  unsigned(time.wDay), unsigned(time.wHour),
                  unsigned(time.wMinute), unsigned(time.wSecond));
  return text;
}
// Worker-only session. The returned handle is closed under the existing log
// lock by telemetry. Re-enabling the same loaded module appends to its own run.
class Session {
  std::wstring selected;
  DWORD failureCode{};
  const wchar_t *failureStage = L"none";
  HANDLE fail(const wchar_t *stage, DWORD error, const std::wstring &path) {
    failureCode = error;
    failureStage = stage;
    wchar_t message[1024]{};
    std::swprintf(
        message, 1024,
        L"WallSprint: workspace logs failure stage=%ls Win32Error=%lu "
        L"path=%.512ls; debug output only; shared logs fallback disabled\n",
        stage, error, path.c_str());
    OutputDebugStringW(message);
    return INVALID_HANDLE_VALUE;
  }

public:
  const std::wstring &path() const noexcept { return selected; }
  DWORD error() const noexcept { return failureCode; }
  const wchar_t *stage() const noexcept { return failureStage; }
  HANDLE open(const std::wstring &executable) {
    SYSTEMTIME local{};
    GetLocalTime(&local);
    return openAt(executable, local);
  }
  HANDLE openAt(const std::wstring &executable, const SYSTEMTIME &time,
                unsigned lastSuffix = 9999) {
    failureCode = 0;
    failureStage = L"none";
    if (!selected.empty()) {
      auto file =
          CreateFileW(selected.c_str(), FILE_APPEND_DATA, FILE_SHARE_READ,
                      nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
      return file == INVALID_HANDLE_VALUE
                 ? fail(L"reopen_owned_run", GetLastError(), selected)
                 : file;
    }
    const auto root = rootDirectory(executable);
    if (root.empty())
      return fail(L"game_root", ERROR_INVALID_NAME, executable);
    auto folder = root;
    for (const auto part :
         {L"Mods Library", L"Mods-Developing", L"MSM2-WS", L"logs"}) {
      folder += L"\\";
      folder += part;
      if (!CreateDirectoryW(folder.c_str(), nullptr)) {
        const DWORD error = GetLastError();
        if (error != ERROR_ALREADY_EXISTS)
          return fail(L"create_workspace_log_directory", error, folder);
      }
      const auto attributes = GetFileAttributesW(folder.c_str());
      if (attributes == INVALID_FILE_ATTRIBUTES ||
          !(attributes & FILE_ATTRIBUTE_DIRECTORY) ||
          (attributes & FILE_ATTRIBUTE_REPARSE_POINT))
        return fail(L"create_workspace_log_directory", ERROR_DIRECTORY, folder);
    }
    if (lastSuffix > 9999)
      lastSuffix = 9999;
    for (unsigned suffix = 0; suffix <= lastSuffix; ++suffix) {
      auto candidate = folder + L"\\" + filename(time, suffix);
      auto file =
          CreateFileW(candidate.c_str(), GENERIC_WRITE, FILE_SHARE_READ,
                      nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
      if (file != INVALID_HANDLE_VALUE) {
        selected = std::move(candidate);
        return file;
      }
      const DWORD error = GetLastError();
      if (error != ERROR_FILE_EXISTS && error != ERROR_ALREADY_EXISTS)
        return fail(L"create_timestamped_file", error, candidate);
    }
    return fail(L"collision_limit", ERROR_FILE_EXISTS, folder);
  }
};
} // namespace ws::logging
