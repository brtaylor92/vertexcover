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
  MinCover() = delete;
  MinCover(istream &is)
      : in(is), N(*(in)), M(*(++in)), G(N * N, false), backups(N) {
    for (int32_t i = 0; i < M; ++i) {
      int32_t v1 = *(++in), v2 = *(++in);
      G.at(v1 + v2 * N) = true;
      G.at(v2 + v1 * N) = true;
    }
    min_soln = min(M, N);
  }
  MinCover(const MinCover &other) = delete;
  MinCover(MinCover &&other) = delete;
  ~MinCover() = default;
  int32_t findMin() {
    int32_t deg = -1;
    int32_t v1 = -1;
    int32_t v2 = -1;
    for (int32_t i = 0; i < N * N; ++i) {
      if (G.at(i)) {
        int32_t new_v1 = i / N;
        int32_t new_v2 = i % N;
        int32_t new_deg =
            count(begin(G) + new_v1 * N, begin(G) + (new_v1 + 1) * N, true) +
            count(begin(G) + new_v2 * N, begin(G) + (new_v2 + 1) * N, true);
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
    backups.at(d) = G;
    int32_t oldM = M;
    ++sz;
    ++d;
    for (int32_t i = 0; i < N; ++i) {
      if (G.at(i + v * N)) {
        G.at(i + v * N) = false;
        G.at(v + i * N) = false;
        --M;
      }
    }
    bool foundone = false;
    do {
      foundone = false;
      for (int32_t i = 0; i < N; ++i) {
        int32_t deg = count(begin(G) + i * N, begin(G) + (i + 1) * N, true);
        if (deg == 1) {
          auto it = find(begin(G) + i * N, begin(G) + (i + 1) * N, true);
          int32_t neighbor = distance(begin(G) + i * N, it);
          for (int32_t j = 0; j < N; ++j) {
            if (G.at(j + neighbor * N)) {
              G.at(j + neighbor * N) = false;
              G.at(neighbor + j * N) = false;
              --M;
            }
          }
          ++sz;
          foundone = true;
        }
      }
    } while (foundone);
    if (M) {
      if (sz + 1 >= min_soln) {
        G.swap(backups.at(d - 1));
        M = oldM;
        return N;
      }
      int32_t deg = -1;
      int32_t v1 = -1;
      int32_t v2 = -1;
      for (int32_t i = 0; i < N * N; ++i) {
        if (G.at(i)) {
          int32_t new_v1 = i / N;
          int32_t new_v2 = i % N;
          int32_t new_deg =
              count(begin(G) + new_v1 * N, begin(G) + (new_v1 + 1) * N, true) +
              count(begin(G) + new_v2 * N, begin(G) + (new_v2 + 1) * N, true);
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

private:
  istream_iterator<int32_t> in;
  int32_t N;
  int32_t M;
  int32_t min_soln;
  vector<bool> G;
  vector<vector<bool>> backups;
};

int main() {
  MinCover mc(cin);
  cout << mc.findMin() << endl;
}
