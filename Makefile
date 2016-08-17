CXX = clang++-3.8
CXXFLAGS = -std=c++14 -Wall -Wextra -Wshadow -pedantic

.PHONY: clean

vcover: vcover.cc
	$(CXX) $(CXXFLAGS) -O3 -fno-exceptions -fno-rtti -fomit-frame-pointer $< -o $@

adjcover: vcover_adjacency.cc
	$(CXX) $(CXXFLAGS) -O3 -fno-exceptions -fno-rtti -fomit-frame-pointer $< -o $@

debug: vcover.cc
	$(CXX) $(CXXFLAGS) -O3 -g -DDEBUG $< -o $@

test: test.sh correct.dat clean vcover
	./$< ./vcover 1

adjtest: test.sh correct.dat clean adjcover
	./$< ./adjcover 1

format: vcover.cc
	clang-format -i $^

lint: vcover.cc
	cpplint $^

clean:
	rm -rf vcover adjcover debug *.dSYM timeset.dat
