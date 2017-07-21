#ifndef BASE_HPP
#define BASE_HPP

#include <string>
#include <iostream>

using namespace std;

enum class node_role {
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
    switch(role) {
        case node_role::scheduler:
            return "scheduler";
        case node_role::worker:
            return "worker";
        case node_role::server:
            return "server";
    }
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
