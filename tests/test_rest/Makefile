SRC_DIR = ../../src/
A_SRC = rest.cpp
A_HEAD = rest.h
A_OBJS = $(A_SRC_LOC:.cpp=.o)
A_SRC_LOC = $(addprefix $(SRC_DIR), $(A_SRC))

all: $(A_SRC_LOC) rest_unittest

%.o: %.cpp
	g++ -Wall -c -o $@ $<

rest_unittest: $(A_OBJS) ../../src/rest.h
	g++ rest_unittest.cpp -lgtest -lgtest_main -lpthread $^ -o $@

clean:
	rm rest_unittest
	rm $(A_OBJS)
