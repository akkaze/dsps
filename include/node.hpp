#ifndef NODE_HPP
#define NODE_HPP
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

class node : public blocking_actor {
public: 
    node(actor_config& cfg) : blocking_actor(cfg) {}
    node(actor_config& cfg,const node_role& role) : blocking_actor(cfg),role_(role) {} 
protected:
    strong_actor_ptr connect(const std::string& host, uint16_t port) {
        strong_actor_ptr incoming_node;
        auto mm = this->system().middleman().actor_handle();
        this->request(mm, infinite, connect_atom::value, host, port).receive(
                [&](const node_id&, strong_actor_ptr node,
                    const std::set<std::string>& ifs) {
                if (!node) {
                        aout(this) << R"(*** no node found at ")" << host << R"(":)"
                               << port << endl;
                        return;
                    }
                    aout(this) << R"(connected)" << endl;
                    incoming_node = node;
                },
                [&](error& err) {
                    aout(this) << this->system().render(err) << endl;
                }
        );
        return incoming_node;
    }
    void start() {
        actor_manager::get()->spawn(this);
    }
    const node_role& get_role() {
        return role_;
    }
    void set_role(const node_role& role) {
        role_ = role;
    }
private:
    node_role role_;
};
#endif
