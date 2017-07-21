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

#include "actor_manager.hpp"
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
        auto scheduler_host = this->scheduler_host();
        auto scheduler_port = this->scheduler_port(); 
        anon_send(worker_,connect_atom::value,scheduler_host,scheduler_port);
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
        );
    }
private:
    messager messager_;
    uint16_t bound_port_;
    string localhost_;
    actor worker_;
}; 
#endif
