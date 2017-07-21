#ifndef DSPS_HPP
#define DSPS_HPP

#include <string>
#include <memory>
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

//#define DEFAULT_ERROR_HANLDER \
//    [&](error& err) {  \
//        aout(this) << this->system().render(err) << endl; \
//    }
void start(size_t argc,char** argv) {
    actor_manager::get()->init(argc,argv);
    shared_ptr<config> cfg = actor_manager::get()->cfg();
    if(cfg->role() == node_role::scheduler)
        std::unique_ptr<scheduler_node> scheduler(new scheduler_node(cfg)); 
    else if(cfg->role() == node_role::worker)
        std::unique_ptr<worker_node> worker(new worker_node(cfg));
    else if(cfg->role() == node_role::server)
        std::unique_ptr<server_node> server(new server_node(cfg));
}

void stop() {
}
#endif
