ifndef CXX
CXX = g++
endif

INCPATH = -I./include

CFLAGS = -std=c++11 -fPIC -O3 -ggdb -finline-functions $(INCPATH)

all: test

clean:
	rm -rf build $(TEST) tests/*.d

include tests/test.mk
test: $(TEST)
