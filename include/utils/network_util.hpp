#include <string>
#include <boost/asio.hpp>
using boost::asio::ip::tcp;

std::string get_local_ip() {
    boost::asio::io_service io_service;
    tcp::resolver resolver(io_service);
    tcp::resolver::query query(boost::asio::ip::host_name(), "");
    tcp::resolver::iterator iter = resolver.resolve(query);
    tcp::resolver::iterator end; // End marker.
    while (iter != end) {
        tcp::endpoint ep = *iter++;
        if(!ep.address().empty())
            return ep.address();
    }
}
