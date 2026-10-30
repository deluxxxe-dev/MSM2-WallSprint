// Owned Win32 filesystem/process fixture. Never touches an installed game.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cassert>
#include <cstdio>
#include <string>
static SECURITY_ATTRIBUTES sa{sizeof(sa),nullptr,TRUE};
static HANDLE open(const wchar_t* p){return CreateFileW(p,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,&sa,OPEN_ALWAYS,FILE_FLAG_OPEN_REPARSE_POINT,nullptr);}
static std::wstring exe;
static PROCESS_INFORMATION spawn(std::wstring args,bool inherit){STARTUPINFOW si{};si.cb=sizeof(si);PROCESS_INFORMATION pi{};std::wstring cmd=L"\""+exe+L"\" "+args;assert(CreateProcessW(nullptr,cmd.data(),nullptr,nullptr,inherit,0,nullptr,nullptr,&si,&pi));return pi;}
static std::wstring num(HANDLE h){return std::to_wstring(reinterpret_cast<UINT_PTR>(h));}
static HANDLE handle(const wchar_t* p){return reinterpret_cast<HANDLE>(_wcstoui64(p,nullptr,10));}
static void finish(PROCESS_INFORMATION p){assert(WaitForSingleObject(p.hProcess,10000)==WAIT_OBJECT_0);CloseHandle(p.hThread);CloseHandle(p.hProcess);}
int wmain(int argc,wchar_t** argv){
 wchar_t self[32768]{};GetModuleFileNameW(nullptr,self,32768);exe=self;
 if(argc>1&&std::wstring(argv[1])==L"attempt"){HANDLE h=open(argv[2]);if(h!=INVALID_HANDLE_VALUE){CloseHandle(h);return 4;}return GetLastError()==ERROR_SHARING_VIOLATION?0:5;}
 if(argc>1&&std::wstring(argv[1])==L"holder"){HANDLE gate=handle(argv[2]),release=handle(argv[3]),ready=handle(argv[4]),done=handle(argv[5]);SetEvent(ready);assert(WaitForSingleObject(release,10000)==WAIT_OBJECT_0);CloseHandle(gate);SetEvent(done);return 0;}
 if(argc>1&&std::wstring(argv[1])==L"crash"){HANDLE gate=open(argv[2]);assert(gate!=INVALID_HANDLE_VALUE);auto p=spawn(L"holder "+num(gate)+L" "+argv[3]+L" "+argv[4]+L" "+argv[5],true);CloseHandle(p.hThread);CloseHandle(p.hProcess);ExitProcess(21);}
 wchar_t temp[32768]{};GetTempPathW(32768,temp);std::wstring folder=std::wstring(temp)+L"MSM2 lock 日本語 ' "+std::to_wstring(GetCurrentProcessId())+L"-"+std::to_wstring(GetTickCount64());assert(CreateDirectoryW(folder.c_str(),nullptr));std::wstring path=folder+L"\\operation.guard";
 HANDLE gate=open(path.c_str());assert(gate!=INVALID_HANDLE_VALUE);
 auto p=spawn(L"attempt \""+path+L"\"",false);assert(WaitForSingleObject(p.hProcess,10000)==WAIT_OBJECT_0);DWORD exit{};GetExitCodeProcess(p.hProcess,&exit);assert(exit==0);finish(p);puts("PASS: independent Windows process cannot claim active CreateFile gate");
 assert(!DeleteFileW(path.c_str()));assert(!MoveFileExW(path.c_str(),(path+L".moved").c_str(),MOVEFILE_REPLACE_EXISTING));puts("PASS: Windows deny-delete/share semantics block unlink and replacement reclaim");
 HANDLE release=CreateEventW(&sa,TRUE,FALSE,nullptr),ready=CreateEventW(&sa,TRUE,FALSE,nullptr),done=CreateEventW(&sa,TRUE,FALSE,nullptr);assert(release&&ready&&done);
 p=spawn(L"holder "+num(gate)+L" "+num(release)+L" "+num(ready)+L" "+num(done),true);assert(WaitForSingleObject(ready,10000)==WAIT_OBJECT_0);CloseHandle(gate);assert(open(path.c_str())==INVALID_HANDLE_VALUE);SetEvent(release);finish(p);gate=open(path.c_str());assert(gate!=INVALID_HANDLE_VALUE);CloseHandle(gate);puts("PASS: inherited Windows child retains gate after parent handle close; exit releases ownership");
 ResetEvent(release);ResetEvent(ready);ResetEvent(done);
 p=spawn(L"crash \""+path+L"\" "+num(release)+L" "+num(ready)+L" "+num(done),true);assert(WaitForSingleObject(ready,10000)==WAIT_OBJECT_0);finish(p);assert(open(path.c_str())==INVALID_HANDLE_VALUE);SetEvent(release);assert(WaitForSingleObject(done,10000)==WAIT_OBJECT_0);gate=open(path.c_str());assert(gate!=INVALID_HANDLE_VALUE);CloseHandle(gate);puts("PASS: crashed Windows owner cannot release remaining build-child ownership; eventual child close permits reclaim");
 std::wstring link=folder+L"\\linked.guard";
 if(CreateSymbolicLinkW(link.c_str(),path.c_str(),2) && GetFileAttributesW(link.c_str())!=INVALID_FILE_ATTRIBUTES && (GetFileAttributesW(link.c_str())&FILE_ATTRIBUTE_REPARSE_POINT)){
  gate=open(link.c_str());assert(gate!=INVALID_HANDLE_VALUE);FILE_ATTRIBUTE_TAG_INFO tag{};assert(GetFileInformationByHandleEx(gate,FileAttributeTagInfo,&tag,sizeof(tag)));assert(tag.FileAttributes&FILE_ATTRIBUTE_REPARSE_POINT);CloseHandle(gate);DeleteFileW(link.c_str());puts("PASS: opened reparse node is detectable before metadata mutation");
 }else {DeleteFileW(link.c_str());puts("UNAVAILABLE: Windows/Wine did not materialize a reparse node; portable symlink refusal exercised separately");}
 CloseHandle(release);CloseHandle(ready);CloseHandle(done);assert(DeleteFileW(path.c_str()));assert(RemoveDirectoryW(folder.c_str()));return 0;
}
