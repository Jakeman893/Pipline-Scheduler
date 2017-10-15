SRC_DIR = ../../src/
A_SRC = rat.cpp
A_HEAD = rat.h
A_OBJS = $(A_SRC_LOC:.cpp=.o)
A_SRC_LOC = $(addprefix $(SRC_DIR), $(A_SRC))

all: $(A_SRC_LOC) rat.unittest

%.o: %.cpp
	g++ -Wall -c -o $@ $<

rat.unittest: $(A_OBJS) ../../src/rat.h
	g++ rat_unittest.cpp -lgtest -lgtest_main -lpthread $^ -o $@

clean:
	rm rat.unittest
	rm $(A_OBJS)
