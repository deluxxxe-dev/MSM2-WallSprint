#include <windows.h>
#include <stdbool.h>
// Only these two memory faults are converted to a failed read. No global handler.
static int filter(DWORD code) {
 return code==EXCEPTION_ACCESS_VIOLATION||code==EXCEPTION_IN_PAGE_ERROR ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH;
}
bool ws_guarded_call(void (*callback)(void*), void* context) {
 __try {callback(context);return true;}
 __except(filter(GetExceptionCode())) {return false;}
}
