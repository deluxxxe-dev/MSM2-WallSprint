#pragma once
// TEST MODEL ONLY. Sequential, bounded transactions supplied by a test driver.
// Not a concurrent production ledger, dispatch proof, or executable bridge.
#include <array>
#include <cstdint>

namespace ws::vch_search_bridge_model {
struct Identity {
  std::uint64_t thread{}, epoch{}, context{}, exception{}, lease{}, generation{}, sequence{};
};
inline bool operator==(const Identity& a,const Identity& b) {
  return a.thread==b.thread&&a.epoch==b.epoch&&a.context==b.context&&a.exception==b.exception&&
         a.lease==b.lease&&a.generation==b.generation&&a.sequence==b.sequence;
}
inline bool valid(const Identity& a) {
  return a.thread&&a.epoch&&a.context&&a.exception&&a.lease&&a.generation&&a.sequence;
}
enum class State { Free, Reserved, SearchEscaped, DownstreamFinal, Bound, Acknowledged };
struct Ticket {
  Identity id{};
  State state{State::Free};
  unsigned reservedOrder{}, searchOrder{}, downstreamOrder{}, vchOrder{};
  std::uint64_t finalContext{}, token{};
};
struct Ledger {
  std::array<Ticket,4> tickets{};
  bool poison{};
  unsigned used{};
  bool refuse() { poison=true;return false; }
  Ticket* find(const Identity& id) {
    for(auto& t:tickets)if(t.state!=State::Free&&t.id==id)return &t;
    return nullptr;
  }
  bool reserve(const Identity& id,unsigned order) {
    if(poison||!valid(id)||!order||used==tickets.size()||find(id))return refuse();
    // The same context/exception address can be reused only with an independently
    // established new sequence; there is no live-slot recycling in this model.
    for(const auto& t:tickets)if(t.state!=State::Free&&t.id.thread==id.thread&&
       t.id.epoch==id.epoch&&t.id.sequence==id.sequence)return refuse();
    auto& t=tickets[used++];t.id=id;t.reservedOrder=order;t.state=State::Reserved;return true;
  }
  bool escape(const Identity& id,unsigned order) {
    auto* t=find(id);if(poison||!t||t->state!=State::Reserved||order<=t->reservedOrder)return refuse();
    t->searchOrder=order;t->state=State::SearchEscaped;return true;
  }
  bool downstream(const Identity& id,unsigned order,std::uint64_t finalContext,bool execution) {
    auto* t=find(id);if(poison||!t||t->state!=State::SearchEscaped||order<=t->searchOrder||!finalContext||!execution)return refuse();
    t->downstreamOrder=order;t->finalContext=finalContext;t->state=State::DownstreamFinal;return true;
  }
  bool bind(const Identity& id,unsigned order,std::uint64_t finalContext,std::uint64_t token,
            bool callbackObserved,bool prepared,bool published) {
    auto* t=find(id);
    if(poison||!t||t->state!=State::DownstreamFinal||order<=t->downstreamOrder||
       finalContext!=t->finalContext||!token||!callbackObserved||!prepared||!published)return refuse();
    for(const auto& other:tickets)if(other.token==token)return refuse();
    t->vchOrder=order;t->token=token;t->state=State::Bound;return true;
  }
  bool acknowledge(const Identity& id,std::uint64_t token) {
    auto* t=find(id);if(poison||!t||t->state!=State::Bound||!token||t->token!=token)return refuse();
    t->state=State::Acknowledged;return true;
  }
  unsigned pending() const {
    unsigned n=0;for(const auto& t:tickets)if(t.state!=State::Free&&t.state!=State::Acknowledged)++n;return n;
  }
  bool canRetire() const { return !poison&&pending()==0; }
  // Exact marker acknowledgment never establishes PC/code-exit quiescence.
  bool storageReclaimable(bool allRetainedWorkerHandlesSignaled) const {return allRetainedWorkerHandlesSignaled;}
};
} // namespace ws::vch_search_bridge_model
