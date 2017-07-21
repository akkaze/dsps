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

#include "working_node.hpp"

#include "actor_manager.hpp"
using namespace std;
using namespace std::chrono;
using namespace caf;


class message;

class worker_node : public working_node { 
public:
    worker_node(config& cfg) : working_node(cfg) {
    }
    void push(const message& msg) {
        messenger_->send(msg);        
    }
    void pull(message& msg) {
        messenger_->recv(msg);
    }
private:
    messager messager_;
}; 
#endif
