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
struct worker_sate {
    actor scheduler;
    vector<actor> current_servers;
};

class worker_node : public node { 
public:
    worker(config& cfg) : node(cfg) {
        auto worker = actor_manager->get()->spawn(worker_node::worker);
        anon_send(worker,connect_atom::value);
    }
    static void worker(stateful_actor<worker_state>* self) {
        return {
            [=](connect_atom atom) {
                connect_to_scheduler(self);   
            }
            [=](connect_to_opponant_atom atom,
                const string& host,uint16_t port) {
                strong_actor_ptr incoming_node = connect(self,host,port);
                self->state().current_servers.push_back(incoming_node);                      
            }
                   
        };      
    }
    //for initing,we need to connect to scheduler
    const actor& connect_to_scheduler(actor* self) {
        const std::string& scheduler_host = this->scheduler_host();
        const uint16_t& scheduler_port = this->scheduler_port();
        auto mm = self->system().middleman();
        auto scheduler = mm.remote_actor(scheduelr_host,scheduler_port);
        CHECK(scheduler) << "unable to connect to node at host: " 
                << scheduler_host << "port :" << scheduler_port
                << self->system().render(scheduler.error()); 
    }
}; 
#endif
