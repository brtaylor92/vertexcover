#include <algorithm>
using std::max_element;
using std::min;

#include <iostream>
using std::cin;
using std::cout;
using std::istream;

#include <iterator>
using std::distance;
using std::istream_iterator;

#include <vector>
using std::vector;

#include "Empirical/tools/BitVector64.h"
using emp::BitVector;

class MinCover {
public:
  MinCover(istream &is)
      : in(is), N(*in), M(*++in), minSoln(N - 1), G(N * N), deg(N),
        backupGs(N), backupDegs(N) {
    for (int32_t i = 0; i < M; ++i) {
      int32_t v1 = *++in, v2 = *++in;
      G.Set(v1 + v2 * N, true);
      G.Set(v2 + v1 * N, true);
      ++deg.at(v1);
      ++deg.at(v2);
    }
  }
  ~MinCover() = default;
  int32_t findCover(int32_t d = 0, int32_t sz = 0) {
    // Remove cliques/degree one vertices; these must always be in the cover
    while (int32_t numRemoved = removeClique()) {
      sz += numRemoved;
    }
    // Check for completion
    if (!M) {
      minSoln = min(sz, minSoln);
      return sz;
    }
    // Find the highest degree vertex
    int32_t v = distance(begin(deg), max_element(begin(deg), end(deg)));
    // If M/d(v) is larger than the best cover, bound
    if (M / deg.at(v) + sz + 1 >= minSoln) {
      return N;
    }
    // Save the graph and related information before removing optional vertices
    int32_t oldM = M;
    backupDegs.at(d) = deg;
    backupGs.at(d++) = G;
    // Consider the highest degree vertex as the best next choice of cover
    vector<int32_t> best(1, v);
    int32_t bestDeg = deg.at(v);
    // Check if there is better branch taking the neighbors of deg 2 vertices
    for (int i = 0; i < N; ++i) {
      if (deg.at(i) == 2) {
        vector<int32_t> current;
        current.reserve(2);
        int32_t currentDeg = 0;
        for (int j = 0; j < N; ++j) {
          if (G.Get(j + i * N)) {
            current.push_back(j);
            currentDeg += deg.at(j);
          }
        }
        if (currentDeg > bestDeg) {
          best = current;
          bestDeg = currentDeg;
        }
      }
    }
    for (auto i : best) {
      removeVertex(i);
    }
    int32_t likely = findCover(d, sz + best.size());
    // Restore those vertices before taking the other branch
    M = oldM;
    deg.swap(backupDegs.at(d - 1));
    G = backupGs.at(d - 1);
    // Look at the neighbors of the "best" cover additions for the other branch
    // If the "best" choices are not part of the min cover, their neighbors are
    for (auto i : best) {
      for (int32_t j = 0; j < N; ++j) {
        if (G.Get(j + i * N)) {
          sz += removeVertex(j);
        }
      }
    }
    // Return the size of the best cover found
    return min(likely, findCover(d, sz));
  }
  bool formsClique(int32_t v) {
    vector<int32_t> n;
    n.reserve(deg.at(v));
    for (int32_t i = 0; i < N; ++i) {
      if (G.Get(i + v * N)) {
        n.push_back(i);
      }
    }
    for (auto i : n) {
      if (deg.at(i) < deg.at(v)) {
        return false;
      }
      for (auto j : n) {
        if (i != j && !G.Get(j + i * N)) {
          return false;
        }
      }
    }
    return true;
  }
  int32_t removeClique() {
    int32_t removed = 0;
    for (int32_t i = 0; i < N; ++i) {
      if (deg.at(i) && formsClique(i)) {
        for (int32_t j = 0; j < N; ++j) {
          if (G.Get(j + i * N)) {
            removed += removeVertex(j);
          }
        }
      }
    }
    return removed;
  }
  int32_t removeVertex(int32_t v) {
    for (int i = 0; i < N; ++i) {
      if (G.Get(i + v * N)) {
        G.Set(i + v * N, false);
        G.Set(v + i * N, false);
        --deg.at(v);
        --deg.at(i);
        --M;
      }
    }
    return 1;
  }

private:
  istream_iterator<int32_t> in;
  int32_t N, M, minSoln;
  BitVector G;
  vector<int32_t> deg;
  vector<BitVector> backupGs;
  vector<vector<int32_t>> backupDegs;
};

int main() { cout << MinCover(cin).findCover() << "\n"; }
