TEST_SRC = $(wildcard tests/test_*.cpp)
TEST = $(patsubst tests/test_%.cpp, tests/test_%, $(TEST_SRC))

LDFLAGS = -Wl,-rpath,-pthread -lcaf_io -lcaf_core
tests/% : tests/%.cpp ./include/*.hpp
	$(CXX) -std=c++0x $(CFLAGS) -MM -MT tests/$* $< >tests/$*.d
	$(CXX) -std=c++0x $(CFLAGS) -o $@ $(filter %.cpp, $^) $(LDFLAGS)
