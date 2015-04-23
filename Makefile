CXX = clang++
CXXFLAGS = -std=c++11 -Wall -Wextra -Wshadow -pedantic

proj: proj.cc Empirical/tools/BitVector64.h
	$(CXX) $(CXXFLAGS) -O3 $< -o $@

debug: proj.cc
	$(CXX) $(CXXFLAGS) -O0 -g -DDEBUG $< -o $@

soln: Empirical/utils/graphs/vcover.cc
	$(CXX) $(CXXFLAGS) -O3 $< -o $@

test: test.sh correct.dat clean proj
	./$< ./proj 3.0s

format: proj.cc
	clang-format -i $^

clean:
	rm -rf proj debug *.dSYM timeset.dat
