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
#include "utils/network_util.hpp"

#include "actor_manager.hpp"
using namespace std;
using namespace std::chrono;
using namespace caf;

class node {
public:
    node() {
        cfg_ = actor_manager::get()->cfg();      
    }
    //virtual ~node() {
    //    stop();
    //} 
public:
    template <class IncomingActor,class Actor = actor>
    static const Actor& connect(IncomingActor* self,const std::string& host, uint16_t port) {
        auto incoming_node = self->system().middleman().remote_actor(host,port);
        CHECK(incoming_node)<< "unable to connect to node at host: " 
                << host << "port :" << port
                << self->system().render(incoming_node.error()) << endl;
        return *incoming_node;
    }
    template <class Actor = actor>
    void publish(const Actor& self,uint16_t port = 0) {
        auto expected_port = actor_manager::get()->system()
            ->middleman().publish(self,port);
        CHECK(expected_port) << actor_manager::get()->system()
            ->render(expected_port.error());
        bound_port_ = *expected_port;
        localhost_ = get_local_ip();
    }
    //set actor role before everything started
    void set_role() {
        scoped_actor blk_atr{*(actor_manager::get()->system())};
        auto role = this->role();
        auto localhost = this->localhost();
        auto bound_port = this->bound_port();
        blk_atr->request(working_actor_,infinite,
            set_role_atom::value,role,
            localhost,bound_port);
    }
    //stop working actor
    void quit() {
        scoped_actor blk_atr{*(actor_manager::get()->system())};
        blk_atr->request(working_actor_,infinite,quit_atom::value);
    }
    void ask_for_blocking(const block_group& group) {
        scoped_actor blk_atr{*(actor_manager::get()->system())};
        blk_atr->request(working_actor_,infinite,ask_for_block_atom::value,group);
        blk_atr->receive(
            [&](const continue_atom ){
                aout(blk_atr) << "continue" << endl;
            },
            [&](const error& err) {
                aout(blk_atr)  << blk_atr->system().render(err) << endl;
            }
        );
    }
    node_role role() const {
        return cfg_->role();
    }
    std::string scheduler_host() const {
        return cfg_->scheduler_host();
    }
    uint16_t scheduler_port() const {
        return cfg_->scheduler_port();
    }
    uint16_t bound_port() const {
        return bound_port_;
    }
    string localhost() const {
        return localhost_;
    }
    void set_working_actor(actor& working_actor) {
        working_actor_ = working_actor;
    }
    const actor& working_actor() const {
        return working_actor_;
    }
    template <class State>
    static message_handler common_message_handler(stateful_actor<State>* self) {
        return {
            [=](set_role_atom atom,node_role role,
                string localhost,uint16_t bound_port) {
                self->state.role = role;
                self->state.localhost = localhost;
                self->state.bound_port = bound_port;
            },
            [=](quit_atom atom) {
                self->quit();
            } 
        };
    }
    template <class State>
    static message_handler ask_block_handler(stateful_actor<State>* self) {
        return {
            [=](ask_for_block_atom atom,const block_group& group) {
                auto role = self->state.role;
                auto node_id = self->state.node_id; 
                auto sender = self->current_sender();
                self->state.blk_atr = sender;
                if(self->state.role != node_role::scheduler) {
                    LOG(INFO) << to_string(role) << " [" << node_id 
                        << "] is blocked in block group: " << to_string(group); 
                    self->request(self->state.scheduler,
                        infinite,block_atom::value,group);
                }
                else {
                    LOG(INFO) << "scheduler is blocked in block group: " << to_string(group);
                    self->request(actor_cast<actor>(self),
                        infinite,block_atom::value,group);
                }
            },
            [=](continue_atom atom) {
                self->request(actor_cast<actor>(self->state.blk_atr),
                    infinite,continue_atom::value);
            }
        };
    }
private:
    std::shared_ptr<config> cfg_;
    actor working_actor_;
    uint16_t bound_port_;
    string localhost_;
};
#endif
