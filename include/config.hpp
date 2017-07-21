#ifndef COFIG_HPP
#define COFIG_HPP

#include "base.hpp"

#include "caf/all.hpp"


using namespace std;
using namespace caf;

class config : public actor_system_config{
public:
  uint16_t scheduler_port_ = 8000;
  std::string scheduler_host_ = "localhost";
  string role_;
public:
  config() {
        opt_group{custom_options_, "global"}
        .add(role_, "node_role", "(worker|server|scheduler)")
        .add(scheduler_port_,"scheduler_port")
        .add(scheduler_host_,"scheduler_host");
  }
  uint16_t scheduler_port() const {
    return scheduler_port_;
  }
  string scheduler_host() const {
    return scheduler_host_;
  }
  node_role role() const {
    return from_string(role_);
  }
  void set_scheduler_port(uint16_t scheduler_port){
    scheduler_port_ = scheduler_port;
  }
  void set_scheduler_host(string scheduler_host) {
    scheduler_host_ = scheduler_host;
  }
};
#endif
