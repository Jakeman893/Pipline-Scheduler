SRC_DIR = ../../src/
A_SRC = rest.cpp rat.cpp rob.cpp pipeline.cpp exeq.cpp
A_HEAD = rest.h rat.h rob.h pipeline.h trace.h
A_OBJS = $(A_SRC_LOC:.cpp=.o)
A_SRC_LOC = $(addprefix $(SRC_DIR), $(A_SRC))
A_H_LOC = $(addprefix $(SRC_DIR), $(A_HEAD))

all: $(A_SRC_LOC) rename.unittest

%.o: %.cpp
	g++ -g -Wall -c -o $@ $<

rename.unittest: $(A_OBJS) ../../src/*.h
	g++ -g rename_unittest.cpp -lgtest -lgtest_main -lpthread $^ -o $@

clean:
	rm rename.unittest
	rm $(A_OBJS)
