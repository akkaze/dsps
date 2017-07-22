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

#include "messager.hpp"

using namespace std;
using namespace std::chrono;
using namespace caf;


class message_t;
class messager;
class worker_node : public working_node { 
public:
    worker_node(const std::shared_ptr<config>& cfg) : working_node(cfg) {
    }
    void push(const message_t& msg) {
        messager_->send(msg);        
    }
    void pull(message_t& msg) {
        messager_->recv(msg);
    }
private:
    messager* messager_;
}; 
#endif
