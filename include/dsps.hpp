#ifndef DSPS_HPP
#define DSPS_HPP

#include <string>
#include <iostream>
#include <chrono>
#include "caf/all.hpp"
#include "caf/io/all.hpp"

using namespace std;
using namespace std::chrono;
using namespace caf;

#define DEFAULT_ERROR_HANLDER \
    [&](error& err) {  \
        aout(this) << this->system().render(err) << endl; \
    }

class config {
public:
  uint16_t scheduler_port = 0;
  std::string scheduler_host = "localhost";
  node_role role;
public:
  void parse_from_env() {
    scheduler_port = Enviroment::Get()->find("DSPS_SCHEDULER_PORT");
    scheduler_host = Enviroment::Get()->find("DSPS_SCHEDULER_HOST");
    role = from_string(Enviroment::Get()->find("DSPS_ROLE")); 
  };  
}

int start() {
    config cfg;
    cfg.parse_from_env();
    if(cfg.role == node_role::scheduler)
        std::unique_ptr scheduler(new scheduler_node(cfg)); 
    else if(cfg.role == node_role::worker)
        std::unique_ptr worker(new worker_node(cfg));
    else if(cfg.role == node_role::server)
        std::unique_ptr server(new server_node(cfg));
}

int stop
#endif
