T_SRC = $(wildcard tests/test_*.cc)
TEST = $(patsubst tests/test_%.cc, tests/test_%, $(TEST_SRC))

LDFLAGS = -Wl,-rpath,-pthread -lboost_system
tests/% : tests/%.cpp
$(CXX) -std=c++0x $(CFLAGS) -MM -MT tests/$* $< >tests/$*.d
  		$(CXX) -std=c++0x $(CFLAGS) -o $@ $(filter %.cpp, $^) $(LDFLAGS)
