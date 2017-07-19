#ifndef SERVER_HPP
#define SERVER_HPP
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

//struct for worker internal state
struct server_state {
    actor scheduler;
    vector<actor> current_wokrers;
    strong_actor_ptr blk_atr;
};

class server_node : public node { 
public:
    server_node(config& cfg) : node(cfg) {
        server_ = actor_manager->get()->system()->spawn(server_node::server);
        uint16_t bound_port = this->publish(server_,0);
        anon_send(server,connect_atom::value);
    }
    void ask_for_blocking(const block_group& group) {
        scoped_actor blk_atr{actor_manager::get()->system()};
        blk_atr->request(server_,infinite,block_atom::value,group);
        blk_atr->receive(
            [&](const continue_atom ){
                aout(blk_atr) << "continue" << endl;
            },
            [&](const error& err) {
                aout(blk_atr)  << system.render(err) << endl;
            }
    }
    static behavior server(stateful_actor<server_state>* self) {
        return {
            [=](connect_atom atom) {
                auto scheduler_host = this->scheduler_host();
                auto scheduler_port = this->scheduler_port();
                auto scheduler = this->connect(reinpreter_cast<actor*>(self),
                    scheduler_host,scheduler_port);
                self->state().scheduler = scheduler;
            },
            [=](connect_to_opponant_atom atom,
                const string& host,uint16_t port) {
                auto incoming_node = this->connect(
                    reinpreter_cast<actor*>(self),
                    host,port);
                self->state().current_workers.push_back(incoming_node);                      
            },
            //connect to existing servers
            [=](connect_back_atom atom, 
                vector<pair<string,uint16_t>> worker_host_and_ports) {
                for(auto worker_host_and_port : worker_host_and_ports) {
                    auto incoming_node = this->connect(
                    reinpreter_cast<actor*>(self),
                    worker_host_and_port.fist,
                    worker_host_and_port.second);
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
    actor server_;    
}; 
#endif
