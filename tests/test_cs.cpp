#include "caf/all.hpp"

#include "dsps.hpp"

using namespace caf;

using add_atom = atom_constant<atom("add")>;

behavior worker_bhvr(event_based_actor* self) { 
    return {
        [=](add_atom atom) {
            LOG(INFO) << "try to connect";
            auto server = self->system()
                .middleman().remote_actor("localhost",8000);
            self->request(*server,infinite,add_atom::value,node_role::worker);
        }
    };
}
behavior server_bhvr(event_based_actor* self) {
    return {
        [=](add_atom atom,node_role rl) {
            LOG(INFO) << "connected";
        }
    };
}
void run_client() {
    auto work = actor_manager::get()->system()
        ->spawn(worker_bhvr);
    anon_send(work,add_atom::value);
} 

void run_server() {
    auto serv  = actor_manager::get()->system()
        ->spawn(server_bhvr);
    actor_manager::get()->system()
        ->middleman().publish(serv,8000);
}
int main(int argc,char** argv) {
    actor_manager::get()->init(argc,argv);
    auto role = actor_manager::get()->cfg()->role();   
    if(role == node_role::worker)
        run_client();
    else if(role == node_role::server)
        run_server();   
}
