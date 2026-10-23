#pragma once
#include <array>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <type_traits>
namespace ws::interop {
// Readers never lock, wait, or access non-atomic shared bytes. Publishers also
// never wait: a concurrent lifecycle-close publication wins on the separate
// lifecycle gate and either publication may skip while another is in flight.
// Sequential consistency makes the version/word/version validation one total
// order. A caller may retain its last coherent value, subject to identity/age.
template<class T> class AtomicPublication {
 static_assert(std::is_trivially_copyable_v<T>);
 static_assert(std::atomic<std::uint64_t>::is_always_lock_free);
 static constexpr unsigned words=(sizeof(T)+7)/8;
 std::atomic<std::uint64_t> sequence{0};
 std::array<std::atomic<std::uint64_t>,words> data{};
 std::atomic_flag publishing=ATOMIC_FLAG_INIT;
public:
 void publish(const T& value) noexcept {
  if(publishing.test_and_set())return;
  std::array<std::uint64_t,words> raw{};std::memcpy(raw.data(),&value,sizeof(T));
  auto v=sequence.load();sequence.store(v+1);
  for(unsigned i=0;i<words;++i)data[i].store(raw[i]);
  sequence.store(v+2);
  publishing.clear();
 }
 bool read(T& out) const noexcept {
  for(unsigned attempt=0;attempt<8;++attempt){
   auto first=sequence.load();if(first&1)continue;
   std::array<std::uint64_t,words> raw{};for(unsigned i=0;i<words;++i)raw[i]=data[i].load();
   auto last=sequence.load();if(first==last&&!(last&1)){std::memcpy(static_cast<void*>(&out),raw.data(),sizeof(T));return true;}
  }
  return false;
 }
};
}
