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
      : in(is), N(*(in++)), M(*(in++)), G(N * N, false), backups(N) {
    for (int_fast16_t i = 0; i < M; ++i) {
      int_fast16_t v1 = *(in++), v2 = *(in++);
      G.at(v1 + v2 * N) = true;
      G.at(v2 + v1 * N) = true;
    }
    min_soln = min(M, N);
  }
  MinCover(const MinCover &other) = delete;
  MinCover(MinCover &&other) = delete;
  ~MinCover() = default;
  int_fast16_t findMin() {
    for (int i = 0; i < N * N; ++i) {
      if (G.at(i)) {
        auto lb = examineVertex(i / N, 0, 0);
        auto rb = examineVertex(i % N, 0, 0);
        return min(lb, rb);
      }
    }
    return N;
  }
  int_fast16_t examineVertex(int_fast16_t v, int_fast16_t d, int_fast16_t sz) {
    backups.at(d) = G;
    int_fast16_t oldM = M;
    ++sz;
    ++d;
    // Remove the edges covered by this vertex from the graph
    for (int_fast16_t i = 0; i < N; ++i) {
      if (G.at(i + v * N)) {
        G.at(i + v * N) = false;
        G.at(v + i * N) = false;
        --M;
      }
    }
    bool foundone = false;
    do {
      foundone = false;
      for (int_fast16_t i = 0; i < N; ++i) {
        int_fast16_t deg =
            count(begin(G) + i * N, begin(G) + (i + 1) * N, true);
        if (deg == 1) {
          auto it = find(begin(G) + i * N, begin(G) + (i + 1) * N, true);
          int_fast16_t neighbor = distance(begin(G) + i * N, it);
          for (int_fast16_t j = 0; j < N; ++j) {
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
    // If we're in our acceptable range and used this vertex,
    // check if this is a cover
    if (M) {
      // If we've reached the max cover size we can stop
      if (sz + 1 >= min_soln) {
        G.swap(backups.at(d - 1));
        M = oldM;
        return N;
      }
      for (int i = 0; i < N * N; ++i) {
        if (G.at(i)) {
          auto lb = examineVertex(i / N, d, sz);
          auto rb = examineVertex(i % N, d, sz);
          G.swap(backups.at(d - 1));
          M = oldM;
          return min(lb, rb);
        }
      }
    }
    if (sz < min_soln) {
      min_soln = sz;
    }
    G.swap(backups.at(d - 1));
    M = oldM;
    return sz;
  }

private:
  istream_iterator<int> in;
  int_fast16_t N;
  int_fast16_t M;
  int_fast16_t min_soln;
  vector<bool> G;
  vector<vector<bool>> backups;
};

int main() {
  MinCover mc(cin);
  cout << mc.findMin() << endl;
}
