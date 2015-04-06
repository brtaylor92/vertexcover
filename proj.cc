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

class MinCover {
public:
  MinCover(istream &is)
      : in(is), N(*in), M(*++in), minSoln(N), G(N * N, false), degrees(N),
        backupGs(N), backupDegrees(N) {
    for (int32_t i = 0; i < M; ++i) {
      int32_t v1 = *++in, v2 = *++in;
      G.at(v1 + v2 * N) = G.at(v2 + v1 * N) = true;
      ++degrees.at(v1);
      ++degrees.at(v2);
    }
  }
  ~MinCover() = default;
  int32_t addToCover(int32_t d, int32_t sz) {
    // Remove cliques/degree one vertices; this is always useful
    while (int32_t numRemoved = removeClique()) {
      sz += numRemoved;
    }
    // Check for completion
    if (!M) {
      minSoln = min(sz, minSoln);
      return sz;
    }
    // Find the highest degree vertex
    int32_t v =
        distance(begin(degrees), max_element(begin(degrees), end(degrees)));
    // If M/d(v) is larger than the best cover, bound
    if (M / degrees.at(v) + sz >= minSoln) {
      return N;
    }
    // Save the graph and related information before removing other vertices
    int32_t oldM = M;
    backupDegrees.at(d) = degrees;
    backupGs.at(d++) = G;
    // Consider the highest degree vertex as the best next choice for the cover
    vector<int32_t> best(1, v);
    int32_t bestDeg = degrees.at(v);
    // Check if there is better path by taking the neighbors of deg 2 vertices
    for (int i = 0; i < N; ++i) {
      if (degrees.at(i) == 2) {
        vector<int32_t> current;
        current.reserve(2);
        int32_t currentDeg = 0;
        for (int j = 0; j < N; j++) {
          if (G.at(j + i * N)) {
            current.push_back(j);
            currentDeg += degrees.at(j);
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
    int32_t left = addToCover(d, sz + best.size());
    // Restore those vertices before taking the other branch
    M = oldM;
    degrees.swap(backupDegrees.at(d - 1));
    G.swap(backupGs.at(d - 1));
    // Look at the neighbors of the "best" cover additions for the other branch
    // If the "best" choices are not part of the min cover, their neighbors are
    for (auto i : best) {
      for (int32_t j = 0; j < N; ++j) {
        if (G.at(j + i * N)) {
          removeVertex(j);
          ++sz;
        }
      }
    }
    int32_t right = addToCover(d, sz);
    // Return the size of the best cover found
    return min(left, right);
  }
  int32_t removeClique() {
    int32_t removed = 0;
    for (int32_t i = 0; i < N; ++i) {
      if (degrees.at(i) && formsClique(i)) {
        for (int32_t j = 0; j < N; ++j) {
          if (G.at(j + i * N)) {
            ++removed;
            removeVertex(j);
          }
        }
      }
    }
    return removed;
  }
  void removeVertex(int32_t v) {
    for (int i = 0; i < N; i++) {
      if (G.at(i + v * N)) {
        G.at(i + v * N) = G.at(v + i * N) = false;
        --degrees.at(v);
        --degrees.at(i);
        --M;
      }
    }
  }
  bool formsClique(int32_t v) {
    vector<int32_t> n;
    n.reserve(degrees.at(v));
    for (int32_t i = 0; i < N; ++i) {
      if (G.at(i + v * N)) {
        n.push_back(i);
      }
    }
    for (auto i : n) {
      for (auto j : n) {
        if (i != j && !G.at(j + i * N)) {
          return false;
        }
      }
    }
    return true;
  }

private:
  istream_iterator<int32_t> in;
  int32_t N, M, minSoln;
  vector<bool> G;
  vector<int32_t> degrees;
  vector<vector<bool>> backupGs;
  vector<vector<int32_t>> backupDegrees;
};

int main() { cout << MinCover(cin).addToCover(0, 0) << "\n"; }
