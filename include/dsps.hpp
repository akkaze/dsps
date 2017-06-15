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

class config : public actor_system_config {
public:
  uint16_t scheduler_port = 0;
  std::string schduler_host = "localhost";
public:
}
#endif
