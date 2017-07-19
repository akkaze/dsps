#ifndef INTERNAL_ENV_H
#define INTERNAL_ENV_H
#include <cstdlib>
#include <unordered_map>
#include <memory>
#include <string>

class Environment {
public:
    static inline Environment* Get() {
       return _GetSharedRef(nullptr).get();
    }
    static inline std::shared_ptr<Environment> _GetSharedRef() {
        return _GetSharedRef(nullptr);
    }
    static inline Environment* Init(const std::unordered_map<std::string, std::string>& envs) {
        return _GetSharedRef(&envs).get();
    }
    const char* find(const char* k) {
        std::string key(k);
        return kvs.find(key) == kvs.end() ? getenv(k) : kvs[key].c_str();
    }
    static std::shared_ptr<Environment> _GetSharedRef(
      const std::unordered_map<std::string, std::string>* envs) {
        static std::shared_ptr<Environment> inst_ptr(new Environment(envs));
        return inst_ptr;
    }
    template <typename V>
    inline V GetEnv(const char *key,V default_val) {
        const char *val = find(key);
        if(val == nullptr) {
            return default_val;
        } else {
            return atoi(val);
        }
    }
private:
    explicit Environment(const std::unordered_map<std::string, std::string>* envs) {
        if (envs) kvs = *envs;
    }

    std::unordered_map<std::string, std::string> kvs;
};
#endif
