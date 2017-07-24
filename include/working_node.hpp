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
    uint32_t node_id;
    node_role role;
    string localhost;
    uint16_t bound_port;
};

class working_node:public node {
public:
    working_node() {
        auto working_actor = actor_manager::get()->system()->spawn(
            working_node::working_node_bhvr);
        //publish worker on the internet through middleman
        this->publish(working_actor);
        LOG(INFO) << "successfully publish new " << to_string(this->role())
            << " at " << this->bound_port();
        this->set_working_actor(working_actor);
        //set node role
        this->set_role();
        string scheduler_host = this->scheduler_host();
        uint16_t scheduler_port = this->scheduler_port();
        string localhost = this->localhost();
        uint16_t bound_port = this->bound_port();
        node_role role = this->role(); 
        anon_send(working_actor,connect_atom::value,
            scheduler_host,scheduler_port);
        
    }
    
public:
    static behavior working_node_bhvr(
        stateful_actor<working_node_state>* self) {
        message_handler common_routines = node::common_message_handler<working_node_state>(self);
        message_handler working_routines = working_node::working_node_routines(self);
        message_handler block_routines = working_node::block_handler(self);
        message_handler routines = working_routines.or_else(common_routines).or_else(block_routines);
        return routines;
    }
    static message_handler working_node_routines(
        stateful_actor<working_node_state>* self) {
           return {
            [=](connect_atom atom,
                string scheduler_host,uint16_t scheduler_port) {
                auto role = self->state.role;
                LOG(INFO) << "new " << to_string(role) << 
                    " try to connect to scheduler at " << 
                    scheduler_host << ":" << scheduler_port;
                auto scheduler = node::connect(self,
                    scheduler_host,scheduler_port);
                LOG(INFO) << "new " << to_string(role) << 
                    " connected to scheduler";
                self->state.scheduler = scheduler;
                auto localhost = self->state.localhost;
                auto bound_port = self->state.bound_port;
                self->request(scheduler,infinite,fetch_id_atom::value,
                    role,localhost,bound_port);
            },
            //after fetch id,try to connect to oppoents
            [=](fetch_id_atom atom,uint32_t node_id) {
                self->state.node_id = node_id;
                auto role = self->state.role;
                auto localhost = self->state.localhost;
                auto bound_port = self->state.bound_port;
                auto schedulr = self->state.scheduler;
                self->request(schedulr,infinite,
                    connect_to_opponent_atom::value,node_id,
                    localhost,bound_port,role);
            },
            //connect to incoming oppoeant node
           [=](connect_to_opponent_atom atom,uint32_t opponent_id,
               const string& host,uint16_t port,
               node_role role) {
                auto incoming_node = node::connect(self,host,port);
                self->state.opponent_nodes.push_back(incoming_node);
                uint32_t node_id = self->state.node_id;
                LOG(INFO) << "existing " << opponent_role(role) << " ["
                        << opponent_id << "] connected to registering " <<
                        to_string(role) << " [" << node_id << "]";
            },
            //connect to existing opponent nodes      
            [=](connect_back_atom atom,
               vector<pair<string,uint16_t>> server_host_and_ports) {
                uint32_t opponent_id = 1;
                node_role role = self->state.role;
                uint32_t node_id = self->state.node_id;
                for(auto server_host_and_port : server_host_and_ports) {
                    auto incoming_node = node::connect(self,
                    server_host_and_port.first,
                    server_host_and_port.second);
                    self->state.opponent_nodes.push_back(incoming_node);
                    LOG(INFO) << "registering " << to_string(role) << " [" 
                        << node_id << "] connected to existing " <<
                        opponent_role(role) << " [" << opponent_id << "]";
                    opponent_id += 1;
                }
            }
        };
   }
    static message_handler block_handler(stateful_actor<working_node_state>* self) {
        return {
            [=](block_atom atom,const block_group& group) {
                auto role = self->state.role;
                auto node_id = self->state.node_id; 
                auto sender = self->current_sender();
                self->state.blk_atr = sender;
                LOG(INFO) << to_string(role) << " [" << node_id 
                    << "] is blocked in block group: " << to_string(group); 
                self->request(self->state.scheduler,
                    infinite,block_atom::value,group);
            },
            [=](continue_atom atom) {
                self->request(actor_cast<actor>(self->state.blk_atr),
                    infinite,continue_atom::value);
                auto role = self->state.role;
                auto node_id = self->state.node_id;
                LOG(INFO) << to_string(role) << " [" << node_id
                   << "] resumed";
            }
        };
    }

private:
};
#endif
