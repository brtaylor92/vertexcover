#include <algorithm>
using std::min;

#include <iostream>
using std::cin;
using std::cout;
using std::endl;
using std::istream;
using std::ostream;

#include <iterator>
using std::istream_iterator;

#include <numeric>
using std::accumulate;

#include <vector>
using std::vector;

class MinCover {
public:
  MinCover() = delete;
  MinCover(istream &is)
      : in(is), N(*(in++)), M(*(in++)), G(N * N, false),
        backups(N) {
    vector<bool> S(N, false);
    for (int_fast8_t i = 0; i < M; ++i) {
      int_fast8_t v1 = *(in++), v2 = *(in++);
      G.at(v1 + v2 * N) = true;
      // Pick edges, construct set of at most 2x vertices
      // Use this to bound size of solution subsets examined
      // Thanks Prateek =)
      S.at(v1) = true;
      S.at(v2) = true;
    }
    int_fast8_t s_sz = accumulate(begin(S), end(S), 0);
    max_sz = min({M, N, s_sz});
    min_sz = s_sz / 2;
    min_soln = max_sz;
  }
  MinCover(const MinCover &other) = delete;
  MinCover(MinCover &&other) = delete;
  ~MinCover() = default;
  int_fast8_t findMin() {
    return min(examineVertices(0, 0, G, true), examineVertices(0, 0, G, false));
  }
  int_fast8_t examineVertices(int_fast8_t v, int_fast8_t sz,
                              vector<bool> &candidate, bool use) {
    // If we've reached the max cover size we can stop
    if (v == max_sz || (v == max_sz - 1 && !use) || sz >= min_soln) {
      return max_sz;
    }
    //vector<bool> keep(candidate);
    backups.at(v) = candidate;
    if (use) {
      // We are including this vertex, increment the size of the solution
      ++sz;
      // Remove the edges covered by this vertex from the graph
      for (int_fast8_t i = 0; i < N; ++i) {
        candidate.at(i + v * N) = false;
        candidate.at(v + i * N) = false;
      }
    }
    ++v;
    // If we're not up to at least our minimum cover size, or we didn't use
    // this vertex, we already know we need to recurse
    if (sz < min_sz || !use) {
      auto lb = examineVertices(v, sz, candidate, true);
      auto rb = examineVertices(v, sz, candidate, false);
      candidate.swap(backups.at(v - 1));
      return min(lb, rb);
    }
    // If we're in our acceptable range and used this vertex,
    // check if this is a cover
    for (auto i : candidate) {
      // As soon as we find a remaining edge, keep looking
      if (i) {
        auto lb = examineVertices(v, sz, candidate, true);
        auto rb = examineVertices(v, sz, candidate, false);
        candidate.swap(backups.at(v - 1));
        return min(lb, rb);
      }
    }
#ifdef DEBUG
    soln = candidate;
#endif
    if (sz < min_soln) {
      min_soln = sz;
    }
    return sz;
  }
#ifdef DEBUG
  void printSoln() {
    for (int_fast8_t i = 0; i < N; ++i) {
      for (int_fast8_t j = 0; j < N; ++j) {
        cout << soln.at(j + i * N) << " ";
      }
      cout << endl;
    }
  }
  friend ostream &operator<<(ostream &out, const MinCover &mc) {
    for (int_fast8_t i = 0; i < mc.N; ++i) {
      for (int_fast8_t j = 0; j < mc.N; ++j) {
        out << mc.G.at(j + i * mc.N) << " ";
      }
      out << endl;
    }
    return out;
  }
#endif

private:
  istream_iterator<int> in;
  const int_fast8_t N;
  const int_fast8_t M;
  int_fast8_t max_sz;
  int_fast8_t min_sz;
  int_fast8_t min_soln;
  vector<bool> G;
  vector<vector<bool>> backups;
#ifdef DEBUG
  vector<bool> soln;
#endif
};

int main() {
  MinCover mc(cin);
#ifdef DEBUG
  cout << mc << endl;
#endif
  cout << static_cast<int>(mc.findMin()) << endl;
#ifdef DEBUG
  mc.printSoln();
#endif
  return 0;
}
