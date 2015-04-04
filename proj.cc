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
    sz += vertices.size();
    for (auto i : vertices) {
      removeVertex(i);
    }
    while (removeClique(sz))
      ;
    if (M) {
      int32_t lb = N, rb = N, deg = -1, v = -1;
      for (int32_t i = 0; i < N; ++i) {
        int32_t newDeg = degrees.at(i);
        if (newDeg > deg) {
          v = i;
          deg = newDeg;
        }
      }
      vector<int32_t> lv;
      lv.reserve(degrees.at(v));
      for (int32_t i = 0; i < N; ++i) {
        if (G.at(i + v * N)) {
          lv.push_back(i);
        }
      }
      vector<int32_t> rv(1, v);
      int32_t ldeg = 0;
      for (auto i : lv) {
        ldeg += degrees.at(i);
      }
      if (static_cast<int32_t>(lv.size()) + sz < minSoln) {
        lb = removeVertices(lv, d, sz);
      }
      if (M / degrees.at(v) + sz < minSoln) {
        rb = removeVertices(rv, d, sz);
      }
      degrees.swap(backupDegrees.at(d - 1));
      G.swap(backupGs.at(d - 1));
      M = oldM;
      return min(lb, rb);
    }
    if (sz < minSoln) {
      minSoln = sz;
    }
    degrees.swap(backupDegrees.at(d - 1));
    G.swap(backupGs.at(d - 1));
    M = oldM;
    return sz;
  }

  bool removeClique(int32_t &sz) {
    int removed = 0;
    for (int32_t i = 0; i < N; ++i) {
      if (degrees.at(i) != 0 && isClique(i)) {
        for (int32_t j = 0; j < N; ++j) {
          if (G.at(j + i * N)) {
            ++sz;
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
