#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP
#include <string>
#include <iostream>
#include <vector>
#include <chrono>

#include "caf/all.hpp"
#include "caf/io/all.hpp"

#include "config.hpp"
#include "base.hpp"
#include "actions.hpp"
#include "node.hpp"

#include "actor_manager.hpp"

using namespace std;
using namespace caf;

//struct for scheduler internal state
struct scheduler_state {
    vector<pair<actor,pair<string,uint16_t>>> current_servers;
    vector<pair<actor,pair<string,uint16_t>>> current_workers;
    strong_actor_ptr blk_atr;
};

class scheduler_node : public node {
public:
    scheduler_node(shared_ptr<config>& cfg) : node(cfg) {
        auto scheduler = actor_manager::get()->system()->spawn(
            scheduler_node::scheduler_bhvr);
        auto scheduler_port = this->scheduler_port();
        this->set_working_actor(scheduler);
        this->publish(scheduler,scheduler_port);
        LOG(INFO) << "successfully publish new scheduler at " << scheduler_port; 
    }
    void demand_to_block(const block_group& group) {
        auto scheduler = this->working_actor();
        scoped_actor blocking_actor{*(actor_manager::get()->system())};
        blocking_actor->request(scheduler,infinite,
            demand_to_block_atom::value,group);
    }
public:
    static behavior scheduler_bhvr(stateful_actor<scheduler_state>* self) {
        message_handler routines = scheduler_node::scheduler_routines(self);
        return routines;
    }
    static message_handler scheduler_routines(stateful_actor<scheduler_state>* self) {
       return {
            [=](const connect_to_opponent_atom& atom,
                std::string host,uint16_t port,
                node_role role) {
                LOG(INFO) << "new " << to_string(role) 
                    << " try to connect to opponent nodes"; 
                if(role == node_role::worker) {
                    auto worker = node::connect(self,host,port);
                    LOG(INFO) << "scheduler connected to new worker";
                    self->state.current_workers.push_back(
                        make_pair(worker,make_pair(host,port)));
                    vector<pair<string,uint16_t>> server_host_and_ports;
                    for(auto serv : self->state.current_servers) {
                        LOG(INFO) << "new worker try to connect server at " 
                            << host << ":" << port;
                        self->request(actor_cast<actor>(serv.first),
                            infinite,connect_to_opponent_atom::value,
                            host,port,role);
                        server_host_and_ports.push_back(serv.second);
                    }
                    auto sender = self->current_sender();
                    self->request(actor_cast<actor>(sender),infinite,
                        connect_back_atom::value,server_host_and_ports);
                }
                else if(role == node_role::server) {
                    auto server = node::connect(self,host,port);
                    LOG(INFO) << "scheduler connected to new server";
                    self->state.current_servers.push_back(
                        make_pair(server,make_pair(host,port)));
                    vector<pair<string,uint16_t>> worker_host_and_ports;
                    for(auto work : self->state.current_workers) {
                        LOG(INFO) << "new server try to connect worker at "
                            << host << ":" << port;
                        self->request(actor_cast<actor>(work.first),
                            infinite,connect_to_opponent_atom::value,
                            host,port,role);
                        worker_host_and_ports.push_back(work.second);
                    }
                    auto sender = self->current_sender();
                    self->request(actor_cast<actor>(sender),infinite,
                        connect_back_atom::value,worker_host_and_ports);
                }
            }
        };
    }
private:
};
#endif
