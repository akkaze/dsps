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

using connect_to_opponent_atom = atom_constant<atom("connect_to_oppenent")>
using ask_for_blocking_atom = atom_constant<atom("ask_for_blocking")>;
using demand_to_block_atom = atom_constant<atom("demand_to_block")>;
using continue_atom = atom_constant<atom("continue")>;
#endif
