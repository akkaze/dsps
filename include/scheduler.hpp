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
    actor scheduler;
    strong_actor_ptr blk_atr;
    node_role role;
    string localhost;
    uint16_t bound_port;
    uint32_t num_recv_blk_msgs;   
    uint32_t node_id;
};

class scheduler_node : public node {
public:
    scheduler_node() {
        auto scheduler = actor_manager::get()->system()->spawn(
            scheduler_node::scheduler_bhvr);
        auto scheduler_port = this->scheduler_port();
        this->set_working_actor(scheduler);
        //set node role
        this->set_role();
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
        message_handler common_routines = node::common_message_handler<scheduler_state>(self);
        message_handler working_routines = scheduler_node::scheduler_routines(self);
        message_handler block_routines = scheduler_node::block_handler(self);
        message_handler routines = working_routines.or_else(common_routines).or_else(block_routines);
        return routines;
    }
    static message_handler scheduler_routines(stateful_actor<scheduler_state>* self) {
       return {
            [=](const fetch_id_atom& atom,node_role role,
                string host,uint16_t port) {
                if(role == node_role::worker) {
                    uint32_t worker_id = self->state.current_workers.size() + 1;
                    auto worker = node::connect(self,host,port);
                    LOG(INFO) << "scheduler connected to registering worker ["
                        << worker_id << " ]";
                    self->state.current_workers.push_back(
                        make_pair(worker,make_pair(host,port)));
                    auto sender = self->current_sender();
                    self->request(actor_cast<actor>(sender),infinite,fetch_id_atom::value,worker_id);
                }
                else if(role == node_role::server) {
                    uint32_t server_id = self->state.current_servers.size() + 1;
                    auto server = node::connect(self,host,port);
                    LOG(INFO) << "scheduler connected to registering server ["
                        << server_id << " ]";
                    self->state.current_servers.push_back(
                        make_pair(server,make_pair(host,port)));
                    auto sender = self->current_sender();
                    self->request(actor_cast<actor>(sender),infinite,fetch_id_atom::value,server_id);
                }
            },
            [=](const connect_to_opponent_atom& atom,
                uint32_t node_id,
                std::string host,uint16_t port,
                node_role role) {
                if(role == node_role::worker) {
                    vector<pair<string,uint16_t>> server_host_and_ports;
                    for(auto serv : self->state.current_servers) {
                        LOG(INFO) << "registering worker [" << node_id << "] " <<
                            "try to connect existing server at " << host << ":" << port;
                        self->request(actor_cast<actor>(serv.first),
                            infinite,connect_to_opponent_atom::value,node_id,
                            host,port,role);
                        server_host_and_ports.push_back(serv.second);
                    }
                    auto sender = self->current_sender();
                    self->request(actor_cast<actor>(sender),infinite,
                        connect_back_atom::value,server_host_and_ports);
                }
                else if(role == node_role::server) {
                    //LOG(INFO) << "scheduler connected to registering worker[" 
                    //    << server_id << "]";
                    vector<pair<string,uint16_t>> worker_host_and_ports;
                    for(auto work : self->state.current_workers) {
                        LOG(INFO) << "registering server [" << node_id << "] " <<
                            "try to connect existing worker at " << host << ":" << port;
                        self->request(actor_cast<actor>(work.first),
                            infinite,connect_to_opponent_atom::value,node_id,
                            host,port,role);
                        worker_host_and_ports.push_back(work.second);
                    }
                    auto sender = self->current_sender();
                    self->request(actor_cast<actor>(sender),infinite,
                        connect_back_atom::value,worker_host_and_ports);
                }
            },
            
        };
    }
    static message_handler block_handler(stateful_actor<scheduler_state>* self) {
        return {
            [=](const block_atom& atom,const block_group& group) {
                self->state.num_recv_blk_msgs += 1;           
                switch(group) {
                    case block_group::all_workers: { 
                        uint32_t num_workers = self->state.current_workers.size();
                        if(self->state.num_recv_blk_msgs == num_workers) {
                            for(auto work : self->state.current_workers) {
                                self->request(actor_cast<actor>(work.first),
                                    infinite,continue_atom::value);
                            }
                            self->state.num_recv_blk_msgs = 0;
                        }
                        break;
                    }
                    case block_group::all_servers: {
                        uint32_t num_servers = self->state.current_servers.size();
                        if(self->state.num_recv_blk_msgs == num_servers) {
                            for(auto serv : self->state.current_servers) {
                                self->request(actor_cast<actor>(serv.first),
                                    infinite,continue_atom::value);
                            }
                            self->state.num_recv_blk_msgs = 0;               
                        }
                        break;
                    }
                    case block_group::all_servers_and_workers: {
                        uint32_t num_servers_and_workers = self->state.current_workers.size() +
                            self->state.current_servers.size();
                        if(self->state.num_recv_blk_msgs == num_servers_and_workers) {
                            for(auto serv : self->state.current_servers) {
                                self->request(actor_cast<actor>(serv.first),
                                    infinite,continue_atom::value);
                            }
                            for(auto work : self->state.current_workers) {
                                self->request(actor_cast<actor>(work.first),
                                    infinite,block_atom::value,group);
                            }
                            self->state.num_recv_blk_msgs = 0;
                        }
                        break;
                    }
                   case block_group::all_nodes: {
                        if(self->state.current_servers.size() == 0)
                            LOG(INFO) << "scheduler is blocked in block group: " 
                                << to_string(group);
                        uint32_t num_nodes = self->state.current_workers.size() +
                            self->state.current_servers.size() + 1;
                        if(self->state.num_recv_blk_msgs == num_nodes) {
                            for(auto serv : self->state.current_servers) {
                                self->request(actor_cast<actor>(serv.first),
                                    infinite,continue_atom::value);
                            }
                            for(auto work : self->state.current_workers) {
                                self->request(actor_cast<actor>(work.first),
                                    infinite,continue_atom::value);
                            }
                            self->request(actor_cast<actor>(self->state.blk_atr),infinite,continue_atom::value);
                            self->state.num_recv_blk_msgs = 0;
                            LOG(INFO) << "scheduler resumed";
                        }
                        break;
                    }
                }
            }
        };
    }

private:
};
#endif
