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

using namespace std;
using namespace std::chrono;
using namespace caf;

//struct for scheduler internal state
struct scheduler_state {
    vector<pair<actor,pair<string,uint16_t>> current_servers;
    vector<pair<actor,pair<string,uint16_t>> current_workers;
    strong_actor_ptr blk_atr;
};

class scheduler_node : node {
public:
    scheduler_node(node& cfg) : node(cfg) {
        scheduler_ = actor_manager->get()->system()->spawn(scheduler_node::scheduler);
        this->publish(scheduler_,cfg.scheduler_port); 
    }
    void demand_to_block(const block_group& group) {
        scoped_actor blocking_actor{actor_manager->get()->system()};
        blocking_actor.request(scheduer_,infinite,
            demand_to_block_atom::value,group);
    }
    void ask_for_blocking(const block_group& group) {
        scoped_actor blk_atr{actor_manager::get()->system()};
        blk_atr->request(scheduler_,infinite,block_atom::value,group);
        blk_atr->receive(
            [&](const continue_atom ){
                aout(blk_atr) << "continue" << endl;
            },
            [&](const error& err) {
                aout(blk_atr)  << system.render(err) << endl;
            }
    }
    static behavior scheduler(stateful_actor<scheduler_state>* self) {
       return {
            [=](const connect_to_opponant_atom& atom,
                std::string host,uint16_t port,
                node_role role) -> 
                    result<connect_back_atom,vector<pair<string,uint16_t>>>{
                if(node_role == node_role::worker) {
                    auto worker = connect(host,port);
                    self->state().current_workers.push_back(
                        make_pair(worker,make_pair(host,port)));
                    vector<pair<string,uint16_t>> server_host_and_ports;
                    for(auto serv : self->state().current_servers) {
                        self->request(actor_cast<actor>(serv.first),
                            infinite,connect_to_opponant_atom::value,
                            host,port);
                        server_host_and_ports.push_back(serv.second);
                    } 
                    return {connect_back_atom::value,server_host_and_ports};
                }
                else if(node_role == node_role::server) {
                    auto server = connect(host,port);
                    self->state().current_servers.push_back(
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
                 
            },
            [=](const demand_to_block_atom& atom,const block_group& group) {
                switch(group) {
                    case block_group::all_workers:
                        for(auto work : self->state().current_workers) {
                            self->request(actor_cast<actor>(work.first),
                                infinite,block_atom::value,group);
                        }
                        break;
                    case block_group::all_servers:
                        for(auto serv : self->state().current_servers) {
                            self->request(actor_cast<actor>(serv),
                                infinite,block_atom::value,group);
                        }
                        break;
                    case block_group::all_server_and_workers:
                        for(auto serv : self->state().current_servers) {
                            self->request(actor_cast<actor>(serv),
                                infinite,block_atom::value,group);
                        }
                        for(auto work : self->state().current_workers) {
                            self->request(actor_cast<actor>(work),
                                infinite,block_atom::value,group);
                        }
                        break;
                    default:
                        break;
                };
            },
            [=](block_atom atom,const block_group& group) {
                auto sender = self->current_sender();
                self->state.blk_atr = sender;
                self->request(self->state.scheduler,infinite,
                    block_atom::value,group);
            },
            [=](continue_atom atom) {
                self->request(actor_cast<actor>(self->state.blk_atr),
                    infinite,continue_atom::value);
            }
        };
    }
private:
    actor scheduler_;
};
#endif
