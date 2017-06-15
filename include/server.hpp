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

//struct for server internal state
struct server_sate {
    strong_actor_ptr scheduler;
    vector<strong_actor_ptr> current_workers;
};
class server : public node { 
public:
    server(actor_config& cfg) : node(cfg,node_role::server) {
        init();
    }
    void override act() {
        bool running = true;
        this->receive_while(running) (
            [connect_to_opponant_atom atom,
                const string& host,uint16_t port] {
                strong_actor_ptr incoming_node = connect(host,port);
                state_.current_workers.push_back(incoming_node);                      
            }
        );      
    }
    //for initing,we need to connect to scheduler
    void init() {
        auto mm = self->system().middleman().actor_handle();
        self->request(mm, infinite, connect_atom::value, host, port).receive(
            [&](const node_id&, strong_actor_ptr scheduler,
                const std::set<std::string>& ifs) {
                this->state_.scheduler = scheduler;
            }
            [&](error& err) {
                aout(this) << this->system().render(err) << endl;
            }      
    }
    //connect to opponant nodes
    void connect_to_opponant() {
        self->request(actor_cast<actor>(this))
    }
private:
    worker_state state_;
}; 
#endif
