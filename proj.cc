#include <algorithm>
using std::count;
using std::max;
using std::min;

#include <iostream>
using std::cin;
using std::cout;
using std::endl;
using std::istream;

#include <iterator>
using std::istream_iterator;

#include <vector>
using std::vector;

class MinCover {
public:
  MinCover(istream &is)
      : in(is), N(*in), M(*++in), minSoln(N), G(N * N, false), degrees(N),
        backupGs(N, vector<bool>(N)), backupDegrees(N, vector<int32_t>(N)) {
    for (int32_t i = 0; i < M; ++i) {
      int32_t v1 = *++in, v2 = *++in;
      G.at(v1 + v2 * N) = true;
      G.at(v2 + v1 * N) = true;
    }
    for (int32_t i = 0; i < N; ++i) {
      degrees.at(i) = count(begin(G) + i * N, begin(G) + (i + 1) * N, true);
    }
  }
  ~MinCover() = default;
  int32_t findMinCover() {
    vector<int32_t> v;
    return removeVertices(v, 0, 0);
  }
  int32_t removeVertices(vector<int32_t> &vertices, int32_t d, int32_t sz) {
    backupDegrees.at(d) = degrees;
    backupGs.at(d++) = G;
    int32_t oldM = M;
    // Remove selected vertices
    sz += vertices.size();
    for (auto i : vertices) {
      removeVertex(i);
    }
    // Remove cliques/degree one vertices
    while (removeClique(sz))
      ;
    // Check for completion
    if (!M) {
      minSoln = min(sz, minSoln);
      degrees.swap(backupDegrees.at(d - 1));
      G.swap(backupGs.at(d - 1));
      M = oldM;
      return sz;
    }
    // Find the highest degree vertex
    int32_t v = 0;
    for (int32_t i = 1; i < N; ++i) {
      if (degrees.at(i) > degrees.at(v)) {
        v = i;
      }
    }
    // If M/d(v) is larger than the best cover, bound
    if (M / degrees.at(v) + sz >= minSoln) {
      degrees.swap(backupDegrees.at(d - 1));
      G.swap(backupGs.at(d - 1));
      M = oldM;
      return N;
    }
    // Consider the highest degree vertex as the best next choice
    vector<int32_t> best(1, v);
    int32_t bestDeg = degrees.at(v);
    // Check if we can get a better next vertex by selecting neighbors of
    // degree two vertices - this helps with grids
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
    // Look at the neighbors of the "best" vertices for the other branch
    vector<int32_t> neighbors;
    neighbors.reserve(bestDeg);
    for (auto i : best) {
      for (int32_t j = 0; j < N; ++j) {
        if (G.at(j + i * N)) {
          neighbors.push_back(j);
        }
      }
    }
    // Recurse
    auto lb = removeVertices(best, d, sz);
    auto rb = removeVertices(neighbors, d, sz);
    degrees.swap(backupDegrees.at(d - 1));
    G.swap(backupGs.at(d - 1));
    M = oldM;
    return min(lb, rb);
  }
  bool removeClique(int32_t &sz) {
    int32_t removed = 0;
    for (int32_t i = 0; i < N; ++i) {
      if (degrees.at(i) != 0 && isClique(i)) {
        for (int32_t j = 0; j < N; ++j) {
          if (G.at(j + i * N)) {
            ++removed;
            removeVertex(j);
          }
        }
      }
    }
    sz += removed;
    return removed;
  }
  void removeVertex(int32_t v) {
    for (int i = 0; i < N; i++) {
      if (G.at(i + v * N)) {
        G.at(i + v * N) = false;
        G.at(v + i * N) = false;
        --degrees.at(v);
        --degrees.at(i);
        --M;
      }
    }
  }
  bool isClique(int32_t v) {
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

int main() { cout << MinCover(cin).findMinCover() << endl; }
