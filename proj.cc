#include <algorithm>
using std::count;
using std::min;

#include <iostream>
using std::cin;
using std::cout;
using std::endl;
using std::istream;
using std::ostream;

#include <iterator>
using std::istream_iterator;

#include <vector>
using std::vector;

class MinCover {
public:
  MinCover(istream &is)
      : in(is), N(*in), M(*++in), min_soln(N), G(N * N, false), backups(N) {
    for (int32_t i = 0; i < M; ++i) {
      int32_t v1 = *++in, v2 = *++in;
      G.at(v1 + v2 * N) = true;
      G.at(v2 + v1 * N) = true;
    }
  }
  MinCover(const MinCover &other) = delete;
  MinCover(MinCover &&other) = delete;
  ~MinCover() = default;
  int32_t findMin() {
    int32_t deg = -1, v1 = -1, v2 = -1;
    for (int32_t i = 0; i < N * N; ++i) {
      if (G.at(i)) {
        int32_t new_v1 = i / N, new_v2 = i % N;
        int32_t new_deg = getDegree(new_v1) + getDegree(new_v2);
        if (new_deg > deg) {
          v1 = new_v1;
          v2 = new_v2;
          deg = new_deg;
        }
      }
    }
    auto lb = examineVertex(v1, 0, 0);
    auto rb = examineVertex(v2, 0, 0);
    return min(lb, rb);
  }
  int32_t examineVertex(int32_t v, int32_t d, int32_t sz) {
    backups.at(d++) = G;
    int32_t oldM = M;
    ++sz;
    removeVertex(v);
    bool foundone = true;
    while (foundone) {
      foundone = false;
      for (int32_t i = 0; i < N; ++i) {
        int32_t deg = getDegree(i);
        if (deg == 1) {
          ++sz;
          foundone = true;
          auto it = find(begin(G) + i * N, begin(G) + (i + 1) * N, true);
          removeVertex(distance(begin(G) + i * N, it));
        }
      }
    }
    if (M) {
      if (sz + 1 >= min_soln) {
        G.swap(backups.at(d - 1));
        M = oldM;
        return N;
      }
      int32_t deg = -1, v1 = -1, v2 = -1;
      for (int32_t i = 0; i < N * N; ++i) {
        if (G.at(i)) {
          int32_t new_v1 = i / N, new_v2 = i % N;
          int32_t new_deg = getDegree(new_v1) + getDegree(new_v2);
          if (new_deg > deg) {
            v1 = new_v1;
            v2 = new_v2;
            deg = new_deg;
          }
        }
      }
      auto lb = examineVertex(v1, d, sz);
      auto rb = examineVertex(v2, d, sz);
      G.swap(backups.at(d - 1));
      M = oldM;
      return min(lb, rb);
    }
    if (sz < min_soln) {
      min_soln = sz;
    }
    G.swap(backups.at(d - 1));
    M = oldM;
    return sz;
  }
  inline void removeVertex(int32_t v) {
    for (int i = 0; i < N; i++) {
      if (G.at(i + v * N)) {
        G.at(i + v * N) = false;
        G.at(v + i * N) = false;
        --M;
      }
    }
  }
  inline int32_t getDegree(int32_t v) {
    return count(begin(G) + v * N, begin(G) + (v + 1) * N, true);
  }

private:
  istream_iterator<int32_t> in;
  int32_t N, M, min_soln;
  vector<bool> G;
  vector<vector<bool>> backups;
};

int main() {
  MinCover mc(cin);
  cout << mc.findMin() << endl;
}
