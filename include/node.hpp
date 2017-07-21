#ifndef NODE_HPP
#define NODE_HPP
#include <string>
#include <iostream>
#include <vector>
#include <chrono>
#include "caf/all.hpp"
#include "caf/io/all.hpp"

#include "dsps.hpp"
#include "base.hpp"
#include "actions.hpp"

#include "utils/logging.hpp"
using namespace std;
using namespace std::chrono;
using namespace caf;

struct working_node_state {
    actor scheduler;
    vector<actor> opponant_working_node;
    strong_actor_ptr blk_atr;
};
class node {
public:
    node(const config& cfg) {
        cfg_ = cfg;       
    } 
protected:
    template <class Actor = actor>
    static const actor& connect(actor* self,const std::string& host, uint16_t port) {
        auto incoming_node = self->system().middleman().remote_actor(host,port);
        CHECK(incoming_node)<< "unable to connect to node at host: " 
                << host << "port :" << port
                << self->system().render(incoming_node.error());
        return *incoming_node;
    }
    template <class Actor = actor>
    const uint16_t& publish(const actor& self) {
        auto expected_port = self.system().middleman().publish(*self,0);
        CHECK(expected_port) << self->system().render(expected_port.error());
        return *expected_port;
    }
    void ask_for_blocking(const block_group& group) {
        scoped_actor blk_atr{actor_manager::get()->system()};
        blk_atr->request(server_,infinite,block_atom::value,group);
        blk_atr->receive(
            [&](const continue_atom ){
                aout(blk_atr) << "continue" << endl;
            },
            [&](const error& err) {
                aout(blk_atr)  << blk_atr->system.render(err) << endl;
            }
        );
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
protected:
   message_hanlder working_node_routines(
        stateful_actor<working_node_state>* self) {
           [=](connect_atom atom,
                string scheduler_host,uint16_t scheduler_port,
                string local_host,uint16_t bound_port,
                node_role role) {
                auto scheduler = connect(
                    reinterpret_cast<actor*>(self),
                    scheduler_host,scheduler_port);
                self->state.scheduler = scheduler;
                self->request(actor_cast<actor>(scheduler),
                    infinite,connect_to_opponant_atom::value,
                    local_host,bound_port,role
                    );
            },
            //connect to incoming opponant node
            [=](connect_to_opponant_atom atom,
                const string& host,uint16_t port) {
                auto incoming_node = node::connect(
                    reinterpret_cast<actor*>(self),
                    host,port);
                self->state.opponant_nodes.push_back(incoming_node);
            },
            //connct to existing nodes
            [=](connect_back_atom atom,
                vector<pair<string,uint16_t>> server_host_and_ports) {
                for(auto server_host_and_port : server_host_and_ports) {
                    auto incoming_node = node::connect(
                    reinterpret_cast<actor*>(self),
                    server_host_and_port.fist,
                    server_host_and_port.second);
                }
            }
        };
   }
private:
    config cfg_;
};
#endif
