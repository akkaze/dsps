#include <string>
#include <iostream>
#include <chrono>
#include <tuple>
#include <vector>
#include "caf/all.hpp"
#include "caf/io/all.hpp"
using namespace std;
using namespace std::chrono;
using namespace caf;
using namespace caf::io;
using namespace caf::io::network;

std::vector<std::string>
list_addresses(std::initializer_list<protocol> procs,
                                   string interface,
                                   bool include_localhost = false) {
      std::vector<std::string> result;
      interfaces::traverse(procs, [&](const char* name, protocol, bool lo, const char* addr) {
        if (include_localhost || !lo && 
            ((!interface.empty() && interface == name) || interface.empty()) )
            result.emplace_back(addr);    
        });
       return result;
}

std::vector<std::string> list_addresses(protocol proc,string interface,
      bool include_localhost= false) {
      return list_addresses({proc}, interface,include_localhost);
}

string get_local_ip() {
    std::vector<string> addresses = list_addresses(protocol::ipv4,"");
    return addresses.front();
}
