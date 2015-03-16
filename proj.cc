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

#include <utility>
using std::make_pair;
using std::pair;

#include <vector>
using std::vector;

class MinCover {
public:
  MinCover() = delete;
  MinCover(istream &is)
      : in(is), N(*(in++)), M(*(in++)), G(N * N, false), force_in(N, false),
        force_out(N, false), adjacency(N), backups(N) {
    //vector<bool> S(N, false);
    for (int_fast8_t i = 0; i < M; ++i) {
      int_fast8_t v1 = *(in++), v2 = *(in++);
      G.at(v1 + v2 * N) = true;
      G.at(v2 + v1 * N) = true;
      adjacency.at(v1).push_back(v2);
      adjacency.at(v2).push_back(v1);
      // Pick edges, construct set of at most 2x vertices
      // Use this to bound size of solution subsets examined
      // Thanks Prateek =)
      //S.at(v1) = true;
      //S.at(v2) = true;
    }
    //int_fast8_t s_sz = accumulate(begin(S), end(S), 0);
    //max_sz = min({M, N, s_sz});
    max_sz = min(M, N);
    min_sz = 1;//s_sz / 2; // This does not appear to be working
    min_soln = max_sz;
  }
  MinCover(const MinCover &other) = delete;
  MinCover(MinCover &&other) = delete;
  ~MinCover() = default;
  int_fast8_t findMin() {
    for (auto &i : adjacency) {
      degrees.push_back(i.size());
    }
    for (int i = 0; i < N; ++i) {
      if (!degrees.at(i)) {
        force_out.at(i) = true;
      } else if (degrees.at(i) == 1) {
        force_in.at(adjacency.at(i).front()) = true;
        force_out.at(i) = true;
      }
    }
    for (int i = 0; i < N; ++i) {
      order.push_back(make_pair(degrees.at(i), i));
    }
    sort(begin(order), end(order), greater<pair<int_fast8_t, int_fast8_t>>());
#ifdef DEBUG
    cout << "q: " << endl;
    for (int i = 0; i < N; ++i) {
      cout << "<" << (int)order.at(i).first << ", " << (int)order.at(i).second << ">" << endl;
    }
    cout << "force_in: ";
    for (auto i : force_in) {
      cout << i << " ";
    }
    cout << endl;
    cout << "force_out: ";
    for (auto i : force_out) {
      cout << i << " ";
    }
    cout << endl;
#endif
    auto lb = max_sz;
    auto rb = max_sz;
    auto v = order.at(0).second;
    if (!force_out.at(v)) {
      lb = examineVertex(0, 0, true);
    }
    if (!force_in.at(v)) {
      rb = examineVertex(0, 0, false);
    }
    return min(lb, rb);
  }
  int_fast8_t examineVertex(int_fast8_t d, int_fast8_t sz, bool use) {
    // If we've reached the max cover size we can stop
    if (d + (use ? 1 : 2) >= N || sz + 1 >= min_soln) {
      return max_sz;
    }
#ifdef DEBUG
    cout << "operating on vertex " << (int)v << " at depth " << (int)d << endl;
#endif
    backups.at(d) = G;
    auto v = order.at(d).second;
    ++d;
    if (use) {
      // We are including this vertex, increment the size of the solution
      ++sz;
      // Remove the edges covered by this vertex from the graph
      for (int_fast8_t i = 0; i < N; ++i) {
        if (G.at(i + v * N)) {
          G.at(i + v * N) = false;
          G.at(v + i * N) = false;
        }
      }
    }
#ifdef DEBUG
    cout << (*this) << endl;
#endif
    // If we're not up to at least our minimum cover size, or we didn't use
    // this vertex, we already know we need to recurse
    if (!use /*|| sz < min_sz*/) {
      auto lb = max_sz;
      auto rb = max_sz;
      v = order.at(d).second;
      if (!force_out.at(v)) {
        lb = examineVertex(d, sz, true);
      }
      if (!force_in.at(v)) {
      rb = examineVertex(d, sz, false);
      }
      G.swap(backups.at(d - 1));
      return min(lb, rb);
    }
    // If we're in our acceptable range and used this vertex,
    // check if this is a cover
    for (auto i : G) {
      // As soon as we find a remaining edge, keep looking
      if (i) {
        auto lb = max_sz;
        auto rb = max_sz;
        v = order.at(d).second;
        if (!force_out.at(v)) {
          lb = examineVertex(d, sz, true);
        }
        if (!force_in.at(v)) {
          rb = examineVertex(d, sz, false);
        }
        G.swap(backups.at(d - 1));
        return min(lb, rb);
      }
    }
#ifdef DEBUG
    cout << "found solution of size " << (int)sz << endl;
#endif
    if (sz < min_soln) {
      min_soln = sz;
    }
    G.swap(backups.at(d - 1));
    return sz;
  }
#ifdef DEBUG
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
  int_fast8_t N;
  int_fast8_t M;
  int_fast8_t max_sz;
  int_fast8_t min_sz;
  int_fast8_t min_soln;
  vector<bool> G;
  vector<bool> force_in;
  vector<bool> force_out;
  vector<vector<int_fast8_t>> adjacency;
  vector<int_fast8_t> degrees;
  vector<pair<int_fast8_t, int_fast8_t>> order;
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
