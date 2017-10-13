SRC_DIR = ../../src/
A_SRC = rat.cpp rest.cpp rob.cpp
A_OBJS = $(A_SRC_LOC:.cpp=.o)
A_SRC_LOC = $(addprefix $(SRC_DIR), $(A_SRC))

all: $(A_SRC_LOC) test

%.o: %.cpp
	g++ -Wall -c -o $@ $<

test: $(A_OBJS)
	g++ test_a.cpp -lgtest -lgtest_main -lpthread -o test_a

clean:
	rm test_a
	rm $(A_OBJS)
