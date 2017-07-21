#ifndef SERVER_HPP
#define SERVER_HPP
#include <string>
#include <iostream>
#include <vector>

#include "caf/all.hpp"
#include "caf/io/all.hpp"

#include "base.hpp"
#include "actions.hpp"
#include "node.hpp"

#include "working_node.hpp"

#include "actor_manager.hpp"
using namespace std;
using namespace caf;


class server_node : public working_node { 
public:
    server_node(config& cfg) : working_node(cfg) {
    }
private:
}; 
#endif
