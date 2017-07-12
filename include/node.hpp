#ifndef NODE_HPP
#define NODE_HPP
#include <string>
#include <iostream>
#include <vector>
#include <chrono>

#include "caf/all.hpp"
#include "caf/io/all.hpp"

#include "base.hpp"
#include "actions.hpp"

using namespace std;
using namespace std::chrono;
using namespace caf;

class node : public {
public:
    node(const config& cfg) {
        cfg_ = cfg;   
    } 
protected:
    const actor& connect(actor* self,const std::string& host, uint16_t port) {
        auto incoming_node = self->system().middleman().remote_actor(host,port);
        CHECK(incoming_node)<< "unable to connect to node at host: " 
                << host << "port :" << port
                << self->system().render(incoming_node.error());
        return *incoming_node;
    }
    const node_role& role() const {
        return cfg_.role;
    }
    void set_role(const node_role& role) {
        cfg_.role = role;
    }
    const std::string& scheduler_host() const {
        return cfg_.scheduler_host;
    }
    const uint16_t scheduler_port() const {
        return cfg_.scheduler_port;
    }
private:
    config cfg_;
};
#endif
