#include <algorithm>
using std::min;

#include <cmath>
using std::exp2;

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
  MinCover(istream &is) : in(is), N(*(in++)), M(*(in++)), G(N * N, false) {
    /*for (int i = 0; i < N; i++) {
      G.at(i * N + i) = true;
    }*/
    for (int i = 0; i < M; i++) {
      int v1 = *(in++), v2 = *(in++);
      G.at(v2 + v1 * N) = true;
      G.at(v1 + v2 * N) = true;
    }
  }
  MinCover(const MinCover &other) = delete;
  MinCover(MinCover &&other) = default;
  virtual ~MinCover() = default;
  int findMin() {
    vector<bool> tree(N, false);
    return min(examineVertices(0, tree, true), examineVertices(0, tree, false));
  }
  int examineVertices(int vertex, vector<bool> tree, bool use) {
    tree.at(vertex) = use;
    ++vertex;
    vector<bool> test(G);
    for (int i = 0; i < vertex; i++) {
      if (tree.at(i)) {
        for (int j = 0; j < N; j++) {
          test.at(j + i * N) = false;
          test.at(i + j * N) = false;
        }
      }
    }
    if (!accumulate(begin(test), end(test), 0)) {
      return accumulate(begin(tree), end(tree), 0);
    } else if (vertex == N) {
      return N;
    }
    return min(examineVertices(vertex, tree, true),
               examineVertices(vertex, tree, false));
  }
  friend ostream &operator<<(ostream &out, const MinCover &mc) {
    for (int i = 0; i < mc.N; i++) {
      for (int j = 0; j < mc.N; j++) {
        out << mc.G.at(j + i * mc.N) << " ";
      }
      out << endl;
    }
    return out;
  }

private:
  istream_iterator<int> in;
  int N;
  int M;
  vector<bool> G;
};

int main() {
  // Read input
  // istream_iterator<int> in(cin);
  // int N = *(in++), M = *(in++);

  // Pick edges, construct set of at most 2x vertices
  // Use this to bound size of solution subsets examined
  // Thanks Prateek =)
  // set<int> S;

  // Construct adjacency matrix
  // vector<bool> G(N * N, false);
  // All vertices have a "call" with themselves
  /*for (int i = 0; i < N; i++) {
    G.at(i * N + i) = true;
  }
  for (int i = 0; i < M; i++) {
    int v1 = *(in++), v2 = *(in++);
    G.at(v2 + v1 * N) = true;
    G.at(v1 + v2 * N) = true;
    // S.insert(v1);
    // S.insert(v2);
  }*/

  // cout << "Max soln: " << S.size() << endl;
  // cout << "Min soln: " << S.size()/2 << endl;

  // Print adjacency matrix

  // Woo, I don't know how to look at all possible subsets
  // Seriously, who thought letting me write this was a good idea?
  // vector<vector<bool>> possible(exp2(N), vector<bool>(N, false));
  // vector<bool> tree(N, false);
  MinCover mc(cin);
  //cout << mc << endl;
  cout << mc.findMin() << endl;
  return 0;
}
