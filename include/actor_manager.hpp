#ifndef ACTOR_MANAGER_HPP
#define ACTOR_MANAGER_HPP
#include <memory>
#include "caf/all.hpp"
#include "caf/io/all.hpp"

#include "config.hpp"
#include "base.hpp"


using namespace std;
using namespace std::chrono;
using namespace caf;

class actor_manager {
public:
    static inline actor_manager* get() {
        return _get_shared_ref().get();
    }
    //get actor system
    inline std::shared_ptr<actor_system> system() {
        return system_;
    }
private:
    explicit actor_manager() {
        config cfg;
        cfg.load<io::middleman>();
        system_.reset(new actor_system(cfg));
    }
    static std::shared_ptr<actor_manager> _get_shared_ref() {   
        static std::shared_ptr<actor_manager> inst_ptr(new actor_manager());
        return inst_ptr;
    }
    config cfg_;
    std::shared_ptr<actor_system> system_;
};
#endif
