CXX = clang++
CXXFLAGS = -std=c++11 -Wall -Wextra -Wshadow -pedantic

vcover: vcover.cc Empirical/tools/BitVector.h
	$(CXX) $(CXXFLAGS) -O3 $< -o $@

debug: vcover.cc Empirical/tools/BitVector.h
	$(CXX) $(CXXFLAGS) -O0 -g -DDEBUG $< -o $@

soln: Empirical/utils/graphs/vcover.cc
	$(CXX) $(CXXFLAGS) -O3 $< -o $@

test: test.sh correct.dat clean vcover
	./$< ./vcover 3

format: vcover.cc
	clang-format -i $^

clean:
	rm -rf vcover debug *.dSYM timeset.dat
