#include <algorithm>
using std::min;

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

#include <numeric>
using std::accumulate;

#include <set>
using std::set;

#include <utility>
using std::make_pair;
using std::pair;

#include <vector>
using std::vector;

class MinCover {
public:
  MinCover() = delete;
  MinCover(istream &is)
      : in(is), N(*(in++)), M(*(in++)), G(N * N, false), adjacency(N),
        backups(N) {
    // vector<bool> S(N, false);
    for (int_fast16_t i = 0; i < M; ++i) {
      int_fast16_t v1 = *(in++), v2 = *(in++);
      G.at(v1 + v2 * N) = true;
      G.at(v2 + v1 * N) = true;
      adjacency.at(v1).push_back(v2);
      adjacency.at(v2).push_back(v1);
      // Pick edges, construct set of at most 2x vertices
      // Use this to bound size of solution subsets examined
      // Thanks Prateek =)
      // S.at(v1) = true;
      // S.at(v2) = true;
    }
    // int_fast16_t s_sz = accumulate(begin(S), end(S), 0);
    // max_sz = min({M, N, s_sz});
    max_sz = min(M, N);
    min_sz = 1; // s_sz / 2; // This does not appear to be working
    min_soln = max_sz;
  }
  MinCover(const MinCover &other) = delete;
  MinCover(MinCover &&other) = delete;
  ~MinCover() = default;
  int_fast16_t findMin() {
    set<int_fast16_t> s;
    for (int_fast16_t i = 0; i < N; ++i) {
      if (adjacency.at(i).size() == 1) {
        int_fast16_t neighbor = adjacency.at(i).front();
        adjacency.at(i).pop_back();
        adjacency.at(neighbor).clear();
#ifdef DEBUG
        cout << "Found deg 1 vertex " << (int)i;
        cout << " with neighbor " << (int)neighbor << endl;
#endif
        for (int j = 0; j < N; ++j) {
          G.at(j + neighbor * N) = false;
          G.at(neighbor + j * N) = false;
        }
        s.insert(neighbor);
      }
    }
    int_fast16_t sz = s.size();
#ifdef DEBUG
    cout << "after degree-1 pruning: " << endl;
    cout << (*this) << endl;
#endif
    bool soln = true;
    for (auto i : G) {
      if (i) {
        soln = false;
        break;
      }
    }
    if (soln) {
      return sz;
    }
    for (int_fast16_t i = 0; i < N; ++i) {
      order.push_back(make_pair(adjacency.at(i).size(), i));
    }
    sort(begin(order), end(order), greater<pair<int_fast16_t, int_fast16_t>>());
#ifdef DEBUG
    cout << "q: " << endl;
    for (int_fast16_t i = 0; i < N; ++i) {
      cout << "<" << (int)order.at(i).first << ", " << (int)order.at(i).second
           << ">" << endl;
    }
    cout << endl;
#endif
    auto lb = examineVertex(0, sz, true);
    auto rb = examineVertex(0, sz, false);
    return min(lb, rb);
  }
  int_fast16_t examineVertex(int_fast16_t d, int_fast16_t sz, bool use) {
    // If we've reached the max cover size we can stop
    if (d + (use ? 1 : 2) >= N || sz + 1 >= min_soln) {
      return max_sz;
    }
    backups.at(d) = G;
    auto v = order.at(d).second;
    ++d;
    // If we're not up to at least our minimum cover size, or we didn't use
    // this vertex, we already know we need to recurse
    if (!use /*|| sz < min_sz*/) {
      auto lb = examineVertex(d, sz, true);
      auto rb = examineVertex(d, sz, false);
      G.swap(backups.at(d - 1));
      return min(lb, rb);
    }

#ifdef DEBUG
    cout << "operating on vertex " << (int)v << " at depth " << (int)d << endl;
#endif
    // We are including this vertex, increment the size of the solution
    ++sz;
    // Remove the edges covered by this vertex from the graph
    for (int_fast16_t i = 0; i < N; ++i) {
      if (G.at(i + v * N)) {
        G.at(i + v * N) = false;
        G.at(v + i * N) = false;
      }
    }
#ifdef DEBUG
    cout << (*this) << endl;
#endif
    // If we're in our acceptable range and used this vertex,
    // check if this is a cover
    for (auto i : G) {
      // As soon as we find a remaining edge, keep looking
      if (i) {
        auto lb = examineVertex(d, sz, true);
        auto rb = examineVertex(d, sz, false);
        G.swap(backups.at(d - 1));
        return min(lb, rb);
      }
    }
    if (sz < min_soln) {
      min_soln = sz;
    }
    G.swap(backups.at(d - 1));
#ifdef DEBUG
    cout << "found solution of size " << (int)sz << endl;
#endif
    return sz;
  }
#ifdef DEBUG
  friend ostream &operator<<(ostream &out, const MinCover &mc) {
    for (int_fast16_t i = 0; i < mc.N; ++i) {
      for (int_fast16_t j = 0; j < mc.N; ++j) {
        out << mc.G.at(j + i * mc.N) << " ";
      }
      out << endl;
    }
    return out;
  }
#endif

private:
  istream_iterator<int> in;
  int_fast16_t N;
  int_fast16_t M;
  int_fast16_t max_sz;
  int_fast16_t min_sz;
  int_fast16_t min_soln;
  vector<bool> G;
  vector<vector<int_fast16_t>> adjacency;
  vector<pair<int_fast16_t, int_fast16_t>> order;
  vector<vector<bool>> backups;
};

int main() {
  MinCover mc(cin);
#ifdef DEBUG
  cout << mc << endl;
#endif
  cout << static_cast<int>(mc.findMin()) << endl;
  return 0;
}
