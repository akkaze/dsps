#include "actor_manager.hpp"
#include "utils/logging.hpp"

int main(int argc,char** argv) {
    actor_manager::get()->init(argc,argv);
    auto cfg = actor_manager::get()->cfg();
    LOG(INFO) << cfg->scheduler_host();
    return 0;
}
