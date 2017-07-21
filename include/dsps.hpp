#ifndef DSPS_HPP
#define DSPS_HPP

#include <string>
#include <iostream>
#include <chrono>
#include <memory>
#include "caf/all.hpp"
#include "caf/io/all.hpp"

#include "base.hpp"
#include "internal/env.h"

#include "scheduler.hpp"
#include "worker.hpp"
#include "server.hpp"

using namespace std;
using namespace std::chrono;
using namespace caf;

#define DEFAULT_ERROR_HANLDER \
    [&](error& err) {  \
        aout(this) << this->system().render(err) << endl; \
    }

class config : public actor_system_config {
public:
  uint16_t scheduler_port_ = 0;
  std::string scheduler_host_ = "localhost";
  string role_;
public:
  config() {
        //opt_group{custom_options_, "global"}
        //.add(node_role, "node_role", "(worker|server|scheduler)")
        //.add(scheduler_port,"scheduler_port")
        //.add(scheduler_host,"scheduler_host");
  }
  uint16_t scheduler_port() const {
    return scheduler_port;
  }
  string scheduler_host() const {
    return scheduler_host;
  }
  node_role role() const {
    return from_string(role);
  }
};  


void start() {
    config cfg;
    if(cfg.role() == node_role::scheduler)
        std::unique_ptr<scheduler_node> scheduler(new scheduler_node(cfg)); 
    else if(cfg.role() == node_role::worker)
        std::unique_ptr<worker_node> worker(new worker_node(cfg));
    else if(cfg.role() == node_role::server)
        std::unique_ptr<server_node> server(new server_node(cfg));
}

void stop() {
}
#endif
