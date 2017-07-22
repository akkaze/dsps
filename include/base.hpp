#ifndef BASE_HPP
#define BASE_HPP

#include <string>
#include <iostream>

#include "caf/all.hpp"
#include "utils/logging.hpp"

using namespace std;
using namespace caf;

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


enum class block_group {
    all_workers,
    all_servers,
    all_servers_and_workers,
    all_nodes
};

struct message_t {
};
#endif
