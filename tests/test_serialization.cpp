#include "dsps.hpp"

#include "caf/all.hpp"

using namespace caf;
using namespace std;

int main(int argc,char** argv) {
    actor_manager::get()->init(argc,argv);
    auto system = actor_manager::get()->system();
    node_role x = node_role::scheduler;
    vector<char> buf;
    binary_serializer bs{*system, buf};
    auto e = bs(x);
    if (e) {
        std::cerr << "*** unable to serialize node_role: "
              << system->render(e) << std::endl;
        return -1;
    }
    node_role y;
    binary_deserializer bd{*system, buf};
    e = bd(y);
    CHECK_EQ(x,y);
    LOG(INFO) << to_string(y);
}
