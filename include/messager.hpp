#ifndef MESSAGER_HPP
#define MESSAGER_HPP
class messager {
    messager() = default;
    virtual bind(uint16_t local_port) = 0;
    virtual connect(string host,port) = 0;
    virtual send() = 0;
    virtual recv() = 0;
};
#endif
