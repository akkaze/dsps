#ifndef SERVER_HPP
#define SERVER_HPP
#include <string>
#include <iostream>
#include <vector>
#include <chrono>

#include "caf/all.hpp"
#include "caf/io/all.hpp"

#include "base.hpp"
#include "actions.hpp"
#include "node.hpp"

#include "actor_manager.hpp"
using namespace std;
using namespace std::chrono;
using namespace caf;


class server_node : public node { 
public:
    server_node(config& cfg) : node(cfg) {
        server_ = actor_manager->get()->system()->spawn(server_node::server);
        uint16_t bound_port = this->publish(server_,0);
        auto scheduler_host = this->scheduler_host();
        auto scheduler_port = this->scheduler_port();
        anon_send(server,connect_atom::value,scheduler_host,scheduler_port);
    }
private:
    actor server_;    
}; 
#endif
