#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP
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

//struct for scheduler internal state
struct scheduler_state {
    vector<pair<actor,pair<string,uint16_t>>> current_servers;
    vector<pair<actor,pair<string,uint16_t>>> current_workers;
    strong_actor_ptr blk_atr;
};

class scheduler_node : public node {
public:
    scheduler_node(node& cfg) : node(cfg) {
        auto scheduler = actor_manager->get()->system()->spawn(
            scheduler_node::scheduler_bhvr);
        auto scheduler_port = this->scheduler_port();
        this->set_working_actor(scheduler);
        this->publish(scheduler,scheduler_port); 
    }
    void demand_to_block(const block_group& group) {
        scoped_actor blocking_actor{actor_manager->get()->system()};
        blocking_actor->request(schedluer_,infinite,
            demand_to_block_atom::value,group);
    }
protect:
    static behavior scheduler_bhvr(stateful_actor<scheduler_state>* self) {
        message_handler routines = scheduler_routines(self);
        return routines;
    }
    message_handler scheduler_routines(stateful_actor<scheduler_state>* self) {
       return {
            [=](const connect_to_opponant_atom& atom,
                std::string host,uint16_t port,
                node_role role) ->
                    result<connect_back_atom,vector<pair<string,uint16_t>>>{
                if(node_role == node_role::worker) {
                    auto worker = node::connect(host,port);
                    self->state.current_workers.push_back(
                        make_pair(worker,make_pair(host,port)));
                    vector<pair<string,uint16_t>> server_host_and_ports;
                    for(auto serv : self->state.current_servers) {
                        self->request(actor_cast<actor>(serv.first),
                            infinite,connect_to_opponant_atom::value,
                            host,port);
                        server_host_and_ports.push_back(serv.second);
                    }
                    return {connect_back_atom::value,server_host_and_ports};
                }
                else if(node_role == node_role::server) {
                    auto server = node::connect(host,port);
                    self->state.current_servers.push_back(
                        make_pair(server,make_pair(host,port)));
                    vector<pair<string,uint16_t>> worker_host_and_ports;
                    for(auto work : self->state().current_workers) {
                        self->request(actor_cast<actor>(work.first),
                            infinite,connect_to_opponant_atom::value,
                            host,port);
                        worker_host_and_ports.push_back(work.second);
                    }
                    return {connect_back_atom::value,worker_host_and_ports};
                }

            }
        };
    }
private:
    actor scheduler_;
};
#endif
