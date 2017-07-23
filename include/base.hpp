#ifndef BASE_HPP
#define BASE_HPP

#include <string>
#include <iostream>

#include "caf/all.hpp"
#include "utils/logging.hpp"

using namespace std;
using namespace caf;

#define DEFAULT_ERROR_HANLDER \
    [&](error& err) {  \
        aout(this) << this->system().render(err) << endl; \
    }


enum node_role : uint32_t {
    scheduler,
    worker,
    server
};

node_role from_string(string role_string) {
    if(role_string == "SCHEDULER")
        return node_role::scheduler;
    else if(role_string == "WORKER")
        return node_role::worker;
    else if(role_string == "SERVER")
        return node_role::server;
}

string to_string(node_role role) {
     if(role == node_role::scheduler)
            return "scheduler";
     else if(role == node_role::worker)
            return "worker";
     else if(role == node_role::server)
            return "server";
}

string opponent_role(node_role role) {
    if(role == node_role::worker)
            return "server";
     else if(role == node_role::server)
            return "worker";
}
enum class block_group {
    all_workers,
    all_servers,
    all_servers_and_workers,
    all_nodes
};
struct host_and_port {
    string host;
    uint16_t port;
};
template <class Inspector>
typename Inspector::result_type inspect(Inspector& f, host_and_port& x) {
  return f(meta::type_name("addrs"), x.host, x.port);
}
struct message_t {
};
#endif
