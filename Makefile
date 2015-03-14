CXX = clang++
CXXFLAGS = -std=c++11 -Wall -Wextra -Wshadow -pedantic -g

proj: proj.cc
	$(CXX) $(CXXFLAGS) $< -o $@

format: proj.cc
	clang-format -i $^

clean:
	rm -rf proj
