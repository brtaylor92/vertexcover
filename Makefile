CXX = clang++
CXXFLAGS = -std=c++11 -Wall -Wextra -Wshadow -pedantic

proj: proj.cc
	$(CXX) $(CXXFLAGS) -O3 $< -o $@

test: proj.cc
	$(CXX) $(CXXFLAGS) -O0 -g -DDEBUG $< -o $@

format: proj.cc
	clang-format -i $^

clean:
	rm -rf proj test *.dSYM
