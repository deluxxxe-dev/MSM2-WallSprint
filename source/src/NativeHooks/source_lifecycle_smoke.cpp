// Actual production publication with real native writer and concurrent readers.
// Owned tokens only; no game code or relaxed game gate.
#include "source_lifecycle.cpp"
#include <cassert>
#include <cstdio>
#include <array>
extern "C" {std::atomic<unsigned> ws_source_lifecycle_enabled{0};void (*ws_source_lifecycle_callback)(unsigned,std::uintptr_t) noexcept=nullptr;}
static std::atomic<bool> finished{false};static std::atomic<unsigned> reads{0},misses{0};
static DWORD WINAPI reader(void*){
 std::uint64_t previous=0;while(!finished.load()){
  SetLastError(0x5421);ws::native_source::Token t{};bool ok=ws::native_source::lifecycle(t);assert(GetLastError()==0x5421);
  if(ok){assert(t.observed&&t.owner==0x123450&&t.phase==3&&t.updates&&t.updateMs>=t.initMs&&t.acceptedEventEpoch==t.collisionEpoch&&t.updates>=previous);previous=t.updates;++reads;}else ++misses;
 }return 0;
}
int main(){
 using namespace ws::native_source;armLifecycle(true);SetLastError(0x1234);event(0,0x123450);event(1,0x123450);event(2,0x123450);assert(GetLastError()==0x1234);
 std::array<HANDLE,4> readers{};for(auto& thread:readers){thread=CreateThread(nullptr,0,reader,nullptr,0,nullptr);assert(thread);}
 auto start=GetTickCount64();unsigned updates=1;do {event(2,0x123450);++updates;Token t{};assert(lifecycle(t)&&t.updates==updates&&t.thread==GetCurrentThreadId());Sleep(10);}while(GetTickCount64()-start<4100);
 finished=true;for(auto thread:readers){assert(WaitForSingleObject(thread,2000)==WAIT_OBJECT_0);CloseHandle(thread);}assert(reads>1000&&contention.load()==0);
 Token t{};assert(lifecycle(t)&&t.updateMs-t.initMs>=4000);
 // Genuine native writer collision remains fail closed until a new clean Init.
 assert(!lifecycleWriteGate.test_and_set());event(2,0x123450);lifecycleWriteGate.clear();event(2,0x123450);assert(!lifecycle(t)&&t.observed&&t.readStatus==LifecycleRead::WriterCollision&&t.updateMs>=t.initMs);
 event(0,0x123450);event(1,0x123450);event(2,0x123450);assert(lifecycle(t));
 event(2,0x654320);assert(!lifecycle(t)&&t.readStatus==LifecycleRead::InvalidSequence);
 event(0,0x123450);event(1,0x123450);event(2,0x123450);auto generation=token.generation;event(3,0x123450);assert(!lifecycle(t)&&t.phase==0&&t.lastKind==3&&t.generation==generation+1);
 armLifecycle(false);assert(!lifecycle(t)&&t.readStatus==LifecycleRead::Disabled);
 std::printf("PASS: production lifecycle publication sustained >=4s with four concurrent readers (%u reads, %u bounded misses); no native update dropped by a reader; coherent identity/time/update sequence and LastError preserved; genuine writer collision/owner mismatch/Deinit/disable fail closed\n",reads.load(),misses.load());
}
