#pragma once
#include "surface_calibration.hpp"
#include <array>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>

namespace ws::scalar_fields {

// Exact Spider-Man2.exe 2.810.0.0 WallRun Update body already authenticated by
// static_observation_plan.hpp and the verified lifecycle hook. These are the five
// unique 4-byte fields directly read through the preserved WallRun `this` alias
// (RSI) by reachable scalar compare instructions in that exact Update body.
// They are observation candidates only; no engine field semantics are assigned.
inline constexpr char updateFunctionSha[] = "e5c08bcdbc98e28f2b07aebb6a141b317ca740ee1a6a8f6da70752f98b98e40f";
inline constexpr std::array<unsigned,5> offsets{{0x208,0x220,0x238,0x250,0x40c}};
inline constexpr std::array<std::uint32_t,5> schemaBits{{
    0x3ecccccdU, // 0.4f
    0x41400000U, // 12.0f
    0x42960000U, // 75.0f
    0x42c80000U, // 100.0f
    0x42480000U, // 50.0f
}};

inline const char* schemaNumericMatch(std::uint32_t bits) {
    switch(bits){
        case 0x3ecccccdU:return "0.4";
        case 0x41400000U:return "12";
        case 0x42960000U:return "75";
        case 0x42c80000U:return "100";
        case 0x42480000U:return "50";
        default:return "none";
    }
}

struct Field {
    unsigned attempts{},readSamples{},changes{};
    std::array<unsigned,5> schemaHits{};
    std::uint32_t firstBits{},lastBits{};
    std::uint64_t firstMs{},lastMs{};
    float first{},last{},minimum{},maximum{};
    bool haveValue{},finiteOnly{true};
};

class Observer {
    std::array<Field,offsets.size()> fields{};
    std::uint64_t session{},startMs{};
    unsigned changeLogs{};
public:
    void reset(){fields={};session=startMs=0;changeLogs=0;}
    void begin(std::uint64_t id,std::uint64_t ms){fields={};session=id;startMs=ms;changeLogs=0;}
    void observe(unsigned index,bool readable,std::uint32_t bits,std::uint64_t ms,calibration::Logger log){
        if(!session||index>=fields.size())return;
        auto& f=fields[index];++f.attempts;
        if(!readable)return;
        float value{};std::memcpy(&value,&bits,sizeof(value));
        bool changed=f.haveValue&&bits!=f.lastBits;
        if(!f.haveValue){
            f.haveValue=true;f.firstBits=bits;f.first=value;f.firstMs=ms;
            f.minimum=f.maximum=value;
        }else if(std::isfinite(value)){
            if(std::isfinite(f.minimum))f.minimum=std::min(f.minimum,value);else f.minimum=value;
            if(std::isfinite(f.maximum))f.maximum=std::max(f.maximum,value);else f.maximum=value;
        }
        if(!std::isfinite(value))f.finiteOnly=false;
        if(changed)++f.changes;
        f.lastBits=bits;f.last=value;f.lastMs=ms;++f.readSamples;
        for(unsigned i=0;i<schemaBits.size();++i)if(bits==schemaBits[i])++f.schemaHits[i];
        if((f.readSamples==1||changed)&&changeLogs<64){
            ++changeLogs;
            log("wallrun_update_scalar_sample session=%llu offset=0x%x phase=%s sample_ms=%llu elapsed_ms=%llu raw_bits=0x%08x float_value=%.9g exact_schema_numeric_match=%s semantic_mapping=UNPROVEN source=exact_Update_direct_this_read_worker_sample NOT_exact_native_instruction=1 game_write=0 pointer_dereference=0",
                static_cast<unsigned long long>(session),offsets[index],f.readSamples==1?"first":"changed",
                static_cast<unsigned long long>(ms),static_cast<unsigned long long>(ms>=startMs?ms-startMs:0),bits,value,schemaNumericMatch(bits));
        }
    }
    void finish(std::uint64_t id,calibration::Logger log){
        if(id!=session)return;
        for(unsigned i=0;i<fields.size();++i){const auto& f=fields[i];
            log("wallrun_update_scalar_summary session=%llu offset=0x%x attempts=%u read_samples=%u changes=%u stable_bits=%u finite_only=%u first_bits=0x%08x last_bits=0x%08x first=%.9g last=%.9g min=%.9g max=%.9g schema_numeric_hit_counts_0p4_12_75_100_50=(%u,%u,%u,%u,%u) semantic_mapping=UNPROVEN source=exact_Update_direct_this_read_worker_sample NOT_exact_native_instruction=1 game_write=0 pointer_dereference=0",
                static_cast<unsigned long long>(id),offsets[i],f.attempts,f.readSamples,f.changes,unsigned(f.haveValue&&f.changes==0),unsigned(f.finiteOnly),
                f.firstBits,f.lastBits,f.first,f.last,f.minimum,f.maximum,
                f.schemaHits[0],f.schemaHits[1],f.schemaHits[2],f.schemaHits[3],f.schemaHits[4]);
        }
        session=0;
    }
};

} // namespace ws::scalar_fields
