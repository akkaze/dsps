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
struct server_sate {
    strong_actor_ptr scheduler;
    vector<strong_actor_ptr> current_workers;
};
class server : public node { 
public:
    server(config& cfg) : node(cfg) {
        auto messager = actor_manager->get()->spawn(worker::messager_fun);
        anon_send(messager,connect_atom::value);
    }
    static void messager_fun(blocking_actor* self) {
        bool running = true;
        self->receive_while(running) (
            [=](connect_atom atom) {
                connect_to_scheduler(self);   
            }
            [=](connect_to_opponant_atom atom,
                const string& host,uint16_t port) {
                strong_actor_ptr incoming_node = connect(self,host,port);
                this->state_.current_workers.push_back(incoming_node);                      
            }
                   
        );      
    }
    //for initing,we need to connect to scheduler
    void connect_to_scheduler(blocking_actor* self) {
        const std::string& scheduler_host = this->scheduler_host();
        const uint16_t& scheduler_port = this->scheduler_port();
        auto mm = self->system().middleman().actor_handle();
        self->request(mm, infinite, connect_atom::value, scheduler_host, scheduler_port).receive(
            [&](const node_id&, strong_actor_ptr scheduler,
                const std::set<std::string>& ifs) {
                this->state_.scheduler = scheduler;
            }
            [&](error& err) {
                aout(self) << self->system().render(err) << endl;
            }      
    }
private:
    worker_state state_;
}; 
#endif
