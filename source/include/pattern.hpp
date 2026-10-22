#pragma once
#include <cstdint>
#include <cstddef>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
namespace ws {
struct Pattern {
 std::vector<int> bytes;
 explicit Pattern(const char* text){
  std::istringstream in(text);std::string token;
  while(in>>token){
   if(token=="??"||token=="**")bytes.push_back(-1);
   else {if(token.size()!=2||token.find_first_not_of("0123456789abcdefABCDEF")!=std::string::npos)throw std::invalid_argument("invalid AOB");bytes.push_back(std::stoi(token,nullptr,16));}
  }
  if(bytes.empty())throw std::invalid_argument("empty AOB");
 }
};
struct Matches {std::size_t count{};std::uintptr_t only{};};
inline void scan(const std::uint8_t* data,std::size_t size,std::uintptr_t base,const Pattern& pattern,Matches& matches){
 if(size<pattern.bytes.size())return;
 for(std::size_t i=0;i<=size-pattern.bytes.size();++i){
  bool found=true;for(std::size_t j=0;j<pattern.bytes.size();++j)if(pattern.bytes[j]>=0&&data[i+j]!=pattern.bytes[j]){found=false;break;}
  if(found){++matches.count;matches.only=matches.count==1?base+i:0;}
 }
}
}
