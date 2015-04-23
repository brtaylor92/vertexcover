CXX = clang++
CXXFLAGS = -std=c++14 -Wall -Wextra -Wshadow -pedantic

vcover: vcover.cc
	$(CXX) $(CXXFLAGS) -O3 -fno-exceptions -fno-rtti -fomit-frame-pointer $< -o $@

debug: vcover.cc
	$(CXX) $(CXXFLAGS) -O0 -g -DDEBUG $< -o $@

soln: Empirical/utils/graphs/vcover.cc
	$(CXX) $(CXXFLAGS) -O3 $< -o $@

test: test.sh correct.dat clean vcover
	./$< ./vcover 3

format: vcover.cc
	clang-format -i $^

clean:
	rm -rf proj debug *.dSYM timeset.dat
