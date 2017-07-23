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
    node(const std::shared_ptr<config>& cfg) {
        cfg_ = cfg;      
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
    void stop() {
        scoped_actor blk_atr{*(actor_manager::get()->system())};
        blk_atr->request(working_actor_,infinite,quit_atom::value);
    }
    void ask_for_blocking(const block_group& group) {
        scoped_actor blk_atr{*(actor_manager::get()->system())};
        blk_atr->request(working_actor_,infinite,block_atom::value,group);
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
private:
    std::shared_ptr<config> cfg_;
    actor working_actor_;
    uint16_t bound_port_;
    string localhost_;
};
#endif
