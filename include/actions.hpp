#ifndef ACTIONS_HPP
#define ACTIONS_HPP

#include <string>
#include <iostream>
#include <chrono>
#include "caf/all.hpp"
#include "caf/io/all.hpp"

using namespace std;
using namespace std::chrono;
using namespace caf;

using quit_atom = atom_constant<atom("quit")>;
using fetch_id_atom = atom_constant<atom("fetch_id")>;
using set_role_atom = atom_constant<atom("set_role")>;
using connect_to_opponent_atom = atom_constant<atom("conn_opp")>;
using connect_back_atom = atom_constant<atom("conn_back")>;

using ask_for_block_atom = atom_constant<atom("ask_blk")>;
using block_atom = atom_constant<atom("block")>;
using demand_to_block_atom = atom_constant<atom("dmd_to_blk")>;
using continue_atom = atom_constant<atom("continue")>;


#endif
