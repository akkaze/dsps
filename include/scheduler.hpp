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

class scheduler_node : node {
public:
    scheduler_node(node& cfg) : node(cfg) {}
    void demand_to_block(const block_group& group) {
        scoped_actor blocking_actor{actor_manager->get()->system()};
        blocking_actor.request(scheduer_,infinite,block_atom::value,group);
    }

    static behavior scheduler(stateful_actor<scheduler_state>* self) {
       return {
            [&](const connect_to_opponant_atom& atom,
                std::string host,uint16_t port,
                node_role role) {
                if(node_role == node_role::worker) {
                    for(size_t i == 0; i < self->state().current_servers.size(); i++) {
                        self->request(actor_cast<>)
                    }  
            }
            [&](const block_atom& atom,const block_group& group) {
                switch(group) {
                    case block_group::all_workers:
                        for(auto serv : self->state().current_workers) {
                            self->request(actor_cast<actor>(serv),infinite,block_atom::value,)
                        }
                    default:
                        break;
                };
            }
        };
    }
private:
    actor scheduler_;
};
#endif
