CXX = clang++
CXXFLAGS = -std=c++11 -Wall -Wextra -Wshadow -pedantic

proj: proj.cc
	$(CXX) $(CXXFLAGS) -O3 $< -o $@

debug: proj.cc
	$(CXX) $(CXXFLAGS) -O0 -g -DDEBUG $< -o $@

test: test/test.sh clean proj
	./$<

format: proj.cc
	clang-format -i $^

clean:
	rm -rf proj debug *.dSYM out
