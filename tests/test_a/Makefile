GTEST_HOME = ~/gtest
LD_LIBRARY_PATH = $(GTEST_HOME)/lib:$(LD_LIBRARY_PATH)

# all: $(A_SR/C) test

%.o: %.cpp
	g++ -Wall -c -o $@ $<

test:
	g++ test_a.cpp -lgtest -lgtest_main -lpthread