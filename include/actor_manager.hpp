#ifndef ACTOR_MANAGER_HPP
#define ACTOR_MANAGER_HPP
class actor_manager {
public:
    static inline actor_manager* get() {
        return _get_shared_ref(nullptr).get();
    }
    static inline actor_system get_system() {
        return _get_shared_ref(nullptr).system_;
    }
    static inline std::shared_ptr<actor_manager> _get_shared_ref() {
        return _get_shared_ref(nullptr);
    }
private:
    explicit actor_manager(const actor_system* system) {
        if(system) system_ = *system;      
    }
    static std::shared_ptr<actor_manager> _get_shared_ref(const actor_system* system) {
        static std::shared_ptr<actor_manager> inst_ptr(new actor_manager(system));
        return inst_ptr;
    }
    actor_system system_;
}
#endif
