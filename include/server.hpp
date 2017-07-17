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
};

class server_node : public node { 
public:
    server_node(config& cfg) : node(cfg) {
        server_ = actor_manager->get()->spawn(server_node::server);
        anon_send(server,connect_atom::value);
    }
    void ask_for_blocking() {
        scoped_actor blocking_actor{actor_manager->get()->system()};
        blocking_actor.request(server_,infinite,block_atom::value).receive(
            [&](const continue_atom ){
                aout(self) << "continue" << endl;
            },
            [&](const error& err) {
                aout(self)  << system.render(err) << endl;
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
            }
            [=](connect_to_opponant_atom atom,
                const string& host,uint16_t port) {
                auto incoming_node = this->connect(
                    reinpreter_cast<actor*>(self),
                    host,port);
                self->state().current_workers.push_back(incoming_node);                      
            }
            [=](block_atom atom) {
                return self->delegate(self->state().scheduler,block_atom::value);    
            }      
        };      
    }
private:
    actor server_;    
}; 
#endif
