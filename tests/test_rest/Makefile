SRC_DIR = ../../src/
A_SRC = rest.cpp
A_HEAD = rest.h
A_OBJS = $(A_SRC_LOC:.cpp=.o)
A_SRC_LOC = $(addprefix $(SRC_DIR), $(A_SRC))

all: $(A_SRC_LOC) rest.unittest

%.o: %.cpp
	g++ -Wall -c -o $@ $<

rest.unittest: $(A_OBJS) ../../src/rest.h
	g++ rest_unittest.cpp -lgtest -lgtest_main -lpthread $^ -o $@

clean:
	rm rest.unittest
	rm $(A_OBJS)
