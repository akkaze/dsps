#ifndef DSPS_HPP
#define DSPS_HPP

#include <string>
#include <memory>
#include <vector>

#include "caf/all.hpp"
#include "caf/io/all.hpp"

#include "base.hpp"
#include "config.hpp"
#include "internal/env.h"

#include "scheduler.hpp"
#include "worker.hpp"
#include "server.hpp"

using namespace std;
using namespace caf;

node_role my_role() {
    return actor_manager::get()->cfg()->role();
}
void init(size_t argc,char** argv) {
    actor_manager::get()->init(argc,argv);
}

void stop() {
}
#endif
