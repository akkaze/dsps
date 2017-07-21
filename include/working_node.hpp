#ifndef WORKING_NODE_HPP
#define WORKING_NODE_HPP
#include <string>
#include <iostream>
#include <vector>
#include "caf/all.hpp"
#include "caf/io/all.hpp"

#include "config.hpp"
#include "base.hpp"
#include "actions.hpp"

#include "utils/logging.hpp"
using namespace std;
using namespace caf;

struct working_node_state {
    actor scheduler;
    vector<actor> opponent_nodes;
    strong_actor_ptr blk_atr;
};

class working_node:public node {
public:
    working_node(const config& cfg) : node(cfg) {
        auto working_actor = actor_manager::get()->system()->spawn(
            working_node::working_node_bhvr);
        //publish worker on the internet through middleman
        this->publish(working_actor);
        this->set_working_actor(working_actor);
        auto scheduler_host = this->scheduler_host();
        auto scheduler_port = this->scheduler_port();
        auto localhost = this->localhost();
        auto bound_port = this->bound_port();
        auto role = this->node_role(); 
        anon_send(working_actor,connect_atom::value,
            scheduler_host,scheduler_port,
            localhost,bound_port,role);
    }
protected:
    static behavior working_node_bhvr(
        stateful_actor<working_node_state>* self) {
        message_handler routines = working_node_routines(self);
        return routines;
    }
    message_handler working_node_routines(
        stateful_actor<working_node_state>* self) {
           return {
            [=](connect_atom atom,
                string scheduler_host,uint16_t scheduler_port,
                string local_host,uint16_t bound_port,
                node_role role) {
                auto scheduler = connect(
                    reinterpret_cast<actor*>(self),
                    scheduler_host,scheduler_port);
                self->state.scheduler = scheduler;
                self->request(actor_cast<actor>(scheduler),
                    infinite,connect_to_opponent_atom::value,
                    local_host,bound_port,role
                    );
            },
            //connect to incoming oppoeant node
           [=](connect_to_opponent_atom atom,
               const string& host,uint16_t port) {
                auto incoming_node = node::connect(
                    reinterpret_cast<actor*>(self),host,port);
                    self->state.opponent_nodes.push_back(incoming_node);
            },
            //connect to existing opponent nodes      
            [=](connect_back_atom atom,
               vector<pair<string,uint16_t>> server_host_and_ports) {
                for(auto server_host_and_port : server_host_and_ports) {
                    auto incoming_node = node::connect(
                    reinterpret_cast<actor*>(self),
                    server_host_and_port.first,
                    server_host_and_port.second);
                }
            }
        };
   }
private:
    actor working_actor_;
};
#endif
