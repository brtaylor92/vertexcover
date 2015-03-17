#include <algorithm>
using std::count;
using std::min;
using std::remove;
using std::transform;

#include <functional>
using std::greater;

#include <iostream>
using std::cin;
using std::cout;
using std::endl;
using std::istream;
using std::ostream;

#include <iterator>
using std::istream_iterator;

#include <utility>
using std::make_pair;
using std::pair;

#include <vector>
using std::vector;

class MinCover {
public:
  MinCover() = delete;
  MinCover(istream &is)
      : in(is), N(*(in++)), M(*(in++)), G(N * N, false), ignored(N, false),
        backups(N) {
    for (int_fast16_t i = 0; i < M; ++i) {
      int_fast16_t v1 = *(in++), v2 = *(in++);
      G.at(v1 + v2 * N) = true;
      G.at(v2 + v1 * N) = true;
      // Pick edges, construct set of at most 2x vertices
      // Use this to bound size of solution subsets examined
      // Thanks Prateek =)
    }
    max_sz = min(M, N);
    min_soln = max_sz;
  }
  MinCover(const MinCover &other) = delete;
  MinCover(MinCover &&other) = delete;
  ~MinCover() = default;
  int_fast16_t findMin() {
    vector<bool> S(N);
    bool foundone = false;
    do {
      bool foundthistime = false;
      for (int_fast16_t i = 0; i < N; ++i) {
        int_fast16_t deg =
            count(begin(G) + i * N, begin(G) + (i + 1) * N, true);
        if (deg == 1) {
          auto it = find(begin(G) + i * N, begin(G) + (i + 1) * N, true);
          int_fast16_t neighbor = distance(begin(G) + i * N, it);
          for (int j = 0; j < N; ++j) {
            if (G.at(j + neighbor * N) || G.at(neighbor + j * N)) {
              G.at(j + neighbor * N) = false;
              G.at(neighbor + j * N) = false;
              --M;
            }
          }
          S.at(neighbor) = true;
          foundthistime = true;
        }
      }
      foundone = foundthistime;
    } while (foundone);
    int_fast16_t sz = count(begin(S), end(S), true);
    if (!M) {
      return sz;
    }
    int_fast16_t max_so_far = 0;
    int_fast16_t next = 0;
    for (int_fast16_t i = 0; i < N; ++i) {
      int_fast16_t deg = count(begin(G) + i * N, begin(G) + (i + 1) * N, true);
      if (deg > max_so_far) {
        max_so_far = deg;
        next = i;
      }
    }
    auto lb = examineVertex(0, next, sz, true);
    auto rb = examineVertex(0, next, sz, false);
    return min(lb, rb);
  }
  int_fast16_t examineVertex(int_fast16_t d, int_fast16_t v, int_fast16_t sz,
                             bool use) {
    // If we've reached the max cover size we can stop
    if (d + (use ? 1 : 2) >= N || sz + 1 >= min_soln) {
      return max_sz;
    }
    backups.at(d) = G;
    int_fast16_t oldM = M;
    ++d;
    // If we're not up to at least our minimum cover size, or we didn't use
    // this vertex, we already know we need to recurse
    if (!use) {
      ignored.at(v) = true;
      int max_so_far = 0;
      int next = 0;
      for (int_fast16_t i = 0; i < N; ++i) {
        int_fast16_t deg =
            count(begin(G) + i * N, begin(G) + (i + 1) * N, true);
        if (deg > max_so_far && !ignored.at(i)) {
          max_so_far = deg;
          next = i;
        }
      }
      auto lb = examineVertex(d, next, sz, true);
      auto rb = examineVertex(d, next, sz, false);
      ignored.at(v) = false;
      G.swap(backups.at(d - 1));
      M = oldM;
      return min(lb, rb);
    }
    // We are including this vertex, increment the size of the solution
    ++sz;
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
      bool foundthistime = false;
      for (int_fast16_t i = 0; i < N; ++i) {
        int_fast16_t deg =
            count(begin(G) + i * N, begin(G) + (i + 1) * N, true);
        if (deg == 1) {
          auto it = find(begin(G) + i * N, begin(G) + (i + 1) * N, true);
          int_fast16_t neighbor = distance(begin(G) + i * N, it);
          for (int j = 0; j < N; ++j) {
            if (G.at(j + neighbor * N)) {
              G.at(j + neighbor * N) = false;
              G.at(neighbor + j * N) = false;
              --M;
            }
          }
          ++sz;
          foundthistime = true;
        }
      }
      foundone = foundthistime;
    } while (foundone);
    // If we're in our acceptable range and used this vertex,
    // check if this is a cover
    if (M) {
      int max_so_far = 0;
      int next = 0;
      for (int_fast16_t i = 0; i < N; ++i) {
        int_fast16_t deg =
            count(begin(G) + i * N, begin(G) + (i + 1) * N, true);
        if (deg > max_so_far && !ignored.at(i)) {
          max_so_far = deg;
          next = i;
        }
      }
      auto lb = examineVertex(d, next, sz, true);
      auto rb = examineVertex(d, next, sz, false);
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
  istream_iterator<int> in;
  int_fast16_t N;
  int_fast16_t M;
  int_fast16_t max_sz;
  int_fast16_t min_soln;
  vector<bool> G;
  vector<bool> ignored;
  // vector<int_fast16_t> order;
  vector<vector<bool>> backups;
};

int main() {
  MinCover mc(cin);
  cout << mc.findMin() << endl;
  return 0;
}
