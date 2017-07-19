#ifndef WORKER_HPP
#define WORKER_HPP
#include <string>
#include <iostream>
#include <vector>
#include <chrono>

#include "caf/all.hpp"
#include "caf/io/all.hpp"

#include "base.hpp"
#include "actions.hpp"
#include "node.hpp"
using namespace std;
using namespace std::chrono;
using namespace caf;

//struct for worker internal state
struct worker_state {
    actor scheduler;
    vector<actor> current_servers;
    strong_actor_ptr blk_atr;
};

class message;

class worker_node : public node { 
public:
    worker_node(config& cfg) : node(cfg) {
        worker_ = actor_manager::get()->system()->spawn(worker_node::worker);
        //publish worker on the internet through middleman
        this->publish(worker_); 
        localhost_ = get_local_ip();
        anon_send(worker_,connect_atom::value);
    }
    void push(const message& msg) {
        messenger_->send(msg);        
    }
    void pull(message& msg) {
        messenger_->recv(msg);
    }
    void ask_for_blocking(const block_group& group) {
        scoped_actor blk_atr{actor_manager::get()->system()};
        blk_atr->request(worker_,infinite,block_atom::value,group);
        blk_atr->receive(
            [&](const continue_atom ){
                aout(blk_atr) << "continue" << endl;
            },
            [&](const error& err) {
                aout(blk_atr)  << system.render(err) << endl;
            }
    }
    static void worker(stateful_actor<worker_state>* self) {
        return {
            [=](connect_atom atom) {
                auto scheduler_host = this->scheduler_host();
                auto scheduler_port = this->scheduler_port();
                auto scheduler = connect(
                    reinterpret_cast<actor*>(self),
                    host,port);
                self->state.scheduler = scheduler;
                self->request(actor_cast<actor>(scheduler),
                    infinite,connect_to_opponant_atom::value,
                    localhost_,bound_port_,
                    node_role::worker
                    );
            },
            //connect to registering server
            [=](connect_to_opponant_atom atom,
                const string& host,uint16_t port) {
                auto incoming_node = node::connect(
                    reinterpret_cast<actor*>(self),
                    host,port);
                self->state.current_servers.push_back(incoming_node);                      
            },
            //connect to existing servers
            [=](connect_back_atom atom,
                vector<pair<string,uint16_t>> server_host_and_ports) {
                for(auto server_host_and_port : server_host_and_ports) {
                    auto incoming_node = node::connect(
                    reinterpret_cast<actor*>(self),
                    server_host_and_port.fist,
                    server_host_and_port.second);
                }
            },
            [=](block_atom atom,const block_group& group) {
                auto sender = self->current_sender();
                self->state.blk_atr = sender;
                self->request(self->state.scheduler,
                    infinite,block_atom::value,group);    
            }, 
            [=](continue_atom atom) {
                self->request(actor_cast<actor>(self->state.blk_atr),
                    infinite,continue_atom::value); 
            }
        };      
    }
private:
    messager messager_;
    uint16_t bound_port_;
    string localhost_;
    actor worker_;
}; 
#endif
