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
struct scheduler_sate {
    vector<strong_actor_ptr> current_servers;
    vector<strong_actor_ptr> current_workers;
};

class scheduler : node {
public:
    scheduler(actor_config& cfg) : stateful_actor(cfg,node_role::scheduler) {}
    void act() override {
        bool running = true;
        this->receive_while(running) (
            //received the connect to opponant message
            [connect_to_opponant_atom connect,
                const std::string& host, uint16_t port,
                const node_role& role] {
                //firtst connect to incoming actor
                strong_actor_ptr incoming_node = connect(this,host,port);
                //after connected to that actor,next,try to deliver this message to its opponant
                if(role == node_role::worker) {
                    state_.current_workers.push_back(incoming_node);
                    for(auto server : this->state->current_servers)
                        this->delegate(actor_cast<actor>(server),connect,host,port);
                } else if(role == node_role::server) {
                    state_.current_servers.push_back(incoming_node);
                    for(auto work : this->state->current_workers) 
                        this->delegate(actor_cast<actor>(worker),connect,host,port);
                }
            }
        );
    }
    
private:
    scheduler_state state_;
};
#endif
