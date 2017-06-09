#include <string>
#include <iostream>
#include <chrono>
#include "caf/all.hpp"
#include "caf/io/all.hpp"

using namespace std;
using namespace std::chrono;
using namespace caf;

using add_atom = atom_constant<atom("add")>;
using connect_to_opponent_atom = atom_constant<atom("connect_to_oppenent")>
