#ifndef MESSAGER_HPP
#define MESSAGER_HPP
#include "base.hpp"
class messager {
public:    
    messager() = default;
    virtual void bind(uint16_t local_port) = 0;
    virtual void connect(string host,uint16_t port) = 0;
    virtual void send(const message_t& msg) = 0;
    virtual void recv(message_t& msg) = 0;
};
#endif
