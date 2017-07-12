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

using namespace std;
using namespace std::chrono;
using namespace caf;

//struct for worker internal state
struct worker_state {
    actor scheduler;
    vector<actor> current_servers;
};

class worker_node : public node { 
public:
    worker_node(config& cfg) : node(cfg) {
        auto worker = actor_manager->get()->spawn(worker_node::worker);
        anon_send(worker,connect_atom::value);
    }
    static void worker(stateful_actor<worker_state>* self) {
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
                self->state().current_servers.push_back(incoming_node);                      
            }
                   
        };      
    }
}; 
#endif
