#include "wallrun_scalar_observation.hpp"
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

namespace {
std::vector<std::string> lines;

void logger(const char* format,...) {
    char text[4096]{};
    va_list args;
    va_start(args,format);
    std::vsnprintf(text,sizeof(text),format,args);
    va_end(args);
    lines.emplace_back(text);
}

bool contains(const char* needle) {
    for(const auto& line:lines) {
        if(line.find(needle)!=std::string::npos) {
            return true;
        }
    }
    return false;
}
}

int main() {
    ws::scalar_fields::Observer observer;

    observer.observe(0,true,0x41400000U,10,&logger);
    if(!lines.empty()) {
        return 1;
    }

    observer.begin(7,1000);
    observer.observe(0,false,0,1050,&logger);
    observer.observe(0,true,0x41400000U,1100,&logger); // 12.0
    observer.observe(0,true,0x41400000U,1200,&logger); // unchanged
    observer.observe(0,true,0x42960000U,1300,&logger); // 75.0
    observer.finish(7,&logger);

    if(!contains("wallrun_update_scalar_sample session=7 offset=0x208 phase=first")) {
        return 2;
    }
    if(!contains("wallrun_update_scalar_sample session=7 offset=0x208 phase=changed")) {
        return 3;
    }
    if(!contains("exact_schema_numeric_match=12")) {
        return 4;
    }
    if(!contains("exact_schema_numeric_match=75")) {
        return 5;
    }
    if(!contains("wallrun_update_scalar_summary session=7 offset=0x208 attempts=4 read_samples=3 changes=1 stable_bits=0")) {
        return 6;
    }
    if(!contains("schema_numeric_hit_counts_0p4_12_75_100_50=(0,2,1,0,0)")) {
        return 7;
    }
    if(!contains("wallrun_update_scalar_summary session=7 offset=0x220 attempts=0 read_samples=0 changes=0")) {
        return 8;
    }

    const auto before=lines.size();
    observer.observe(0,true,0x41400000U,1400,&logger);
    if(lines.size()!=before) {
        return 9;
    }

    std::puts("wallrun_scalar_observation_tests: PASS");
    return 0;
}
