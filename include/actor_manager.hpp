#ifndef ACTOR_MANAGER_HPP
#define ACTOR_MANAGER_HPP
class actor_manager {
public:
    static inline actor_manager* get() {
        return _get_shared_ref(nullptr).get();
    }
    //get actor system
    inline actor_system system() {
        return system_;
    }
    static inline std::shared_ptr<actor_manager> _get_shared_ref() {
        return _get_shared_ref(nullptr);
    }
private:
    explicit actor_manager() {
        actor_system_config config;
        system_.reset(new actor_system(config));
    }
    static std::shared_ptr<actor_manager> _get_shared_ref() {   
        static std::shared_ptr<actor_manager> inst_ptr(new actor_manager());
        return inst_ptr;
    }
    std::shared_ptr<actor_system> system_;
}
#endif
