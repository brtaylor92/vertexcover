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
    int32_t deg = -1, v1 = -1, v2 = -1;
    auto deg_cmp = [&](int32_t lhs, int32_t rhs) {
      return degrees.at(lhs) < degrees.at(rhs);
    };
    for (int32_t i = 0; i < N * N; ++i) {
      if (G.at(i)) {
        int32_t new_v1 = i / N, new_v2 = i % N;
        int32_t new_deg = degrees.at(new_v1) + degrees.at(new_v2);
        if (new_deg > deg) {
          v1 = max(new_v1, new_v2, deg_cmp);
          v2 = min(new_v1, new_v2, deg_cmp);
          deg = new_deg;
        }
      }
    }
    vector<int32_t> lv;
    for (int32_t i = 0; i < N; ++i) {
      if (G.at(i + v1 * N)) {
        lv.push_back(i);
      }
    }
    vector<int32_t> rv(1, v1);
    auto lb = examineVertex(lv, 0, 0);
    auto rb = examineVertex(rv, 0, 0);
    return min(lb, rb);
  }
  int32_t examineVertex(vector<int32_t> &v, int32_t d, int32_t sz) {
    backup_degrees.at(d) = degrees;
    backups.at(d++) = G;
    int32_t oldM = M;
    for (auto i : v) {
      removeVertex(i);
      ++sz;
    }
    bool foundone = true;
    while (foundone) {
      foundone = false;
      for (int32_t i = 0; i < N; ++i) {
        if (degrees.at(i) == 1) {
          ++sz;
          foundone = true;
          for (int32_t j = 0; j < N; ++j) {
            if (G.at(j + i * N)) {
              removeVertex(j);
              break;
            }
          }
        }
      }
    }
    if (M) {
      int32_t deg = -1, v1 = -1, v2 = -1;
      for (int32_t i = 0; i < N * N; ++i) {
        if (G.at(i)) {
          auto deg_cmp = [&](int32_t lhs, int32_t rhs) {
            return degrees.at(lhs) < degrees.at(rhs);
          };
          int32_t new_v1 = i / N, new_v2 = i % N;
          int32_t new_deg = degrees.at(new_v1) + degrees.at(new_v2);
          if (new_deg > deg) {
            v1 = max(new_v1, new_v2, deg_cmp);
            v2 = min(new_v1, new_v2, deg_cmp);
            deg = new_deg;
          }
        }
      }
      vector<int32_t> lv;
      for (int32_t i = 0; i < N; ++i) {
        if (G.at(i + v1 * N)) {
          lv.push_back(i);
        }
      }
      vector<int32_t> rv(1, v1);
      auto lb = N, rb = N;
      int32_t ldeg = 0;
      for (auto i : lv) {
        ldeg += degrees.at(i);
      }
      if (static_cast<int32_t>(lv.size()) + sz < min_soln) {
        lb = examineVertex(lv, d, sz);
      }
      if (M / degrees.at(v1) + sz < min_soln) {
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
  inline void removeVertex(int32_t v) {
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
