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
struct schduler_sate {
    vector<strong_actor_ptr> current_servers;
    vector<strong_actor_ptr> current_workers;
};
class scheduler : stateful_actor<scheduler_state> {
public:
    scheduler(actor_config& cfg) : event_based_actor(cfg) {
       role_ = role::scheduler; 
       // nop
    }
    static behavior_type receiving() override {
        [connect_to_opponant_atom, connect 
            const std::string& host, uint16_t port,
            const node_role& role] {
            connecting(this,host,port); 
            for(auto serv : this->state->current_servers)
                this->delegate(actor_cast<serv>,connect,host,port,role);
        }
    }
    //connecting routine when the scheduler receive the connect to opponant atom
    void connecting(stateful_actor<scheduler_state>* self,
        const std::string& host, uint16_t port,
        const node_role& role) {
            auto mm = self->system().middleman().actor_handle();
            self->request(mm, infinite, connect_atom::value, host, port).await(
            [=](const node_id&, strong_actor_ptr client,
                const std::set<std::string>& ifs) {
            if (!client) {
                 aout(this) << R"(*** no client found at ")" << host << R"(":)"
                        << port << endl;
                return;
            }
            if (!ifs.empty()) {
                aout(this) << R"(*** typed actor found at ")" << host << R"(":)"
                        << port << ", but expected an untyped actor "<< endl;
                return;
            }
            if(role == node_role::worker) {
                this->state->current_workers.push_back(client);
            }
        );        
    }
private:
    node_role role_;
};
