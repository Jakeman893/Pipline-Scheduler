SRC_DIR = ../../src/
A_SRC = rob.cpp
A_HEAD = rob.h
A_OBJS = $(A_SRC_LOC:.cpp=.o)
A_SRC_LOC = $(addprefix $(SRC_DIR), $(A_SRC))

all: $(A_SRC_LOC) rob.unittest

%.o: %.cpp
	g++ -g -Wall -c -o $@ $<

rob.unittest: $(A_OBJS) ../../src/rob.h
	g++ -g rob_unittest.cpp -lgtest -lgtest_main -lpthread $^ -o $@

clean:
	rm $(A_OBJS)
	rm rob.unittest
