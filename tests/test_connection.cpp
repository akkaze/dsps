#include "dsps.hpp"

int main(int argc,char **argv) {
    init(argc,argv);
    auto role = my_role();
    switch(role) {
        case node_role::scheduler : {
            std::unique_ptr<scheduler_node> scheduler(new scheduler_node());
            std::this_thread::sleep_for(seconds(2));
            scheduler->ask_for_blocking(block_group::all_nodes);
            scheduler->quit();
            return 0;
        }
        case node_role::worker : {
            std::unique_ptr<worker_node> worker(new worker_node());
            std::this_thread::sleep_for(seconds(2));
            worker->ask_for_blocking(block_group::all_nodes);
            worker->quit(); 
            return 0;
        }
        case node_role::server : {
            std::unique_ptr<server_node> server(new server_node());
            std::this_thread::sleep_for(seconds(2));
            server->ask_for_blocking(block_group::all_nodes);
            server->quit();  
            return 0;        
        }
    }
}
