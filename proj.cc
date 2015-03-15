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
      : in(is), N(*(in++)), M(*(in++)), G(N * N, false), force_in(N, false),
        force_out(N, false), adjacency(N), /*b_degrees(N), b_force_out(N),*/ backups(N) {
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
    /*for (int i = 0; i < N; ++i) {
      int_fast8_t sum = 0;
      for (int j = 0; j < N; ++j) {
        if (G.at(j + i * N)) {
          ++sum;
        }
      }
      if (sum <= 1) {
        force_out.at(i) = true;
        for (int j = 0; j < N; ++j) {
          if (G.at(j + i * N)) {
            force_in.at(j) = true;
          }
        }
      }
    }*/
    /*for (auto &i : adjacency) {
      for (auto &j : i) {
        cout << (int)j << " ";
      }
      cout << endl;
    }*/
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
    /*cout << "force out: " << endl;
    for (auto i : force_out) {
      cout << i << " ";
    }
    cout << endl;
    cout << "force in: " << endl;
    for (auto i : force_in) {
      cout << i << " ";
    }
    cout << endl;*/
    auto lb = max_sz;
    auto rb = max_sz;
    if (!force_out.at(0)) {
      lb = examineVertex(0, 0, true);
    }
    if (!force_in.at(0)) {
      rb = examineVertex(0, 0, false);
    }
    return min(lb, rb);
  }
  int_fast8_t examineVertex(int_fast8_t v, int_fast8_t sz, bool use) {
    // If we've reached the max cover size we can stop
    if (v > max_sz || (v == max_sz && !use) || sz >= min_soln) {
      return max_sz;
    }
    backups.at(v) = G;
    //b_degrees.at(v) = degrees;
    //b_force_out.at(v) = force_out;
    if (use) {
      // We are including this vertex, increment the size of the solution
      ++sz;
      // Remove the edges covered by this vertex from the graph
      for (int_fast8_t i = 0; i < N; ++i) {
        if (G.at(i + v * N)) {
          G.at(i + v * N) = false;
          G.at(v + i * N) = false;
          /*if (!force_out.at(i) && !--degrees.at(i)) {
            force_out.at(i) = true;
          }*/
        }
      }
    }
    ++v;
    // If we're not up to at least our minimum cover size, or we didn't use
    // this vertex, we already know we need to recurse
    if (sz < min_sz || !use) {
      auto lb = max_sz;
      auto rb = max_sz;
      if (v != N && !force_out.at(v)) {
        lb = examineVertex(v, sz, true);
      }
      if (v != N && !force_in.at(v)) {
        rb = examineVertex(v, sz, false);
      }
      //force_out.swap(b_force_out.at(v - 1));
      //degrees.swap(b_degrees.at(v - 1));
      G.swap(backups.at(v - 1));
      return min(lb, rb);
    }
    // If we're in our acceptable range and used this vertex,
    // check if this is a cover
    for (auto i : G) {
      // As soon as we find a remaining edge, keep looking
      if (i) {
        auto lb = max_sz;
        auto rb = max_sz;
        if (v != N && !force_out.at(v)) {
          lb = examineVertex(v, sz, true);
        }
        if (v != N && !force_in.at(v)) {
          rb = examineVertex(v, sz, false);
        }
        //force_out.swap(b_force_out.at(v - 1));
        //degrees.swap(b_degrees.at(v - 1));
        G.swap(backups.at(v - 1));
        return min(lb, rb);
      }
    }
    if (sz < min_soln) {
      min_soln = sz;
    }
    //force_out.swap(b_force_out.at(v - 1));
    //degrees.swap(b_degrees.at(v -1));
    G.swap(backups.at(v - 1));
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
  //vector<vector<int_fast8_t>> b_degrees;
  //vector<vector<bool>> b_force_out;
  vector<int_fast8_t> degrees;
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
