CXX = clang++
CXXFLAGS = -std=c++11 -Wall -Wextra -Wshadow -pedantic

proj: proj.cc
	$(CXX) $(CXXFLAGS) -O3 $< -o $@

debug: proj.cc
	$(CXX) $(CXXFLAGS) -O0 -g -DDEBUG $< -o $@

test: MSS/test.sh MSS/correct.dat clean proj
	./$< ./proj 3.0s

format: proj.cc
	clang-format -i $^

clean:
	rm -rf proj debug *.dSYM timeset.dat
