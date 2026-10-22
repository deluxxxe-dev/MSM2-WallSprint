#include <windows.h>
#include <cstdio>
#include <cstdint>
extern "C" bool ws_guarded_call(void (*callback)(void*), void* context);
static void invalidRead(void* address) {volatile auto value=*static_cast<volatile unsigned*>(address);(void)value;}
static void goodRead(void* value) {++*static_cast<int*>(value);}
int main(){
 int count=0;
 if(!ws_guarded_call(goodRead,&count)||count!=1||ws_guarded_call(invalidRead,reinterpret_cast<void*>(1))){std::puts("FAIL SEH guard");return 1;}
 for(int cycle=0;cycle<3;++cycle){
  HMODULE module=LoadLibraryW(L"WallSprint.dll");if(!module){std::printf("FAIL LoadLibrary %lu\n",GetLastError());return 2;}
  auto enable=reinterpret_cast<void (*)()>(GetProcAddress(module,"script_enable"));auto disable=reinterpret_cast<void (*)()>(GetProcAddress(module,"script_disable"));
  if(!enable||!disable)return 3;
  enable();enable();Sleep(1200);disable();disable();enable();Sleep(700);disable();
  if(!FreeLibrary(module))return 4;
 }
 std::puts("PASS: SEH guarded read, DLL load, repeated enable/disable, restart, clean unload (synthetic host only)");return 0;
}
