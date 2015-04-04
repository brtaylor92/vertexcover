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
      : in(is), N(*in), M(*++in), min_soln(N), G(N * N, false), degrees(N),
        backups(N, vector<bool>(N)), backup_degrees(N, vector<int32_t>(N)) {
    for (int32_t i = 0; i < M; ++i) {
      int32_t v1 = *++in, v2 = *++in;
      G.at(v1 + v2 * N) = true;
      G.at(v2 + v1 * N) = true;
    }
    for (int32_t i = 0; i < N; ++i) {
      degrees.at(i) = count(begin(G) + i * N, begin(G) + (i + 1) * N, true);
    }
  }
  MinCover(const MinCover &other) = delete;
  MinCover(MinCover &&other) = delete;
  ~MinCover() = default;
  int32_t findMin() {
    vector<int32_t> v;
    return examineVertex(v, 0, 0);
  }
  int32_t examineVertex(vector<int32_t> &vertices, int32_t d, int32_t sz) {
    backup_degrees.at(d) = degrees;
    backups.at(d++) = G;
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
        int32_t new_deg = degrees.at(i);
        if (new_deg > deg) {
          v = i;
          deg = new_deg;
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
      if (static_cast<int32_t>(lv.size()) + sz < min_soln) {
        lb = examineVertex(lv, d, sz);
      }
      if (M / degrees.at(v) + sz < min_soln) {
        rb = examineVertex(rv, d, sz);
      }
      degrees.swap(backup_degrees.at(d - 1));
      G.swap(backups.at(d - 1));
      M = oldM;
      return min(lb, rb);
    }
    if (sz < min_soln) {
      min_soln = sz;
    }
    degrees.swap(backup_degrees.at(d - 1));
    G.swap(backups.at(d - 1));
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
  int32_t N, M, min_soln;
  vector<bool> G;
  vector<int32_t> degrees;
  vector<vector<bool>> backups;
  vector<vector<int32_t>> backup_degrees;
};

int main() {
  MinCover mc(cin);
  cout << mc.findMin() << endl;
}
