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
    for (int i = 0; i < M; i++) {
      int v1 = *(in++), v2 = *(in++);
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
#ifdef DEBUG
      soln = test;
#endif
      return accumulate(begin(tree), end(tree), 0);
    } else if (vertex == N) {
      return N;
    }
    return min(examineVertices(vertex, tree, true),
               examineVertices(vertex, tree, false));
  }
#ifdef DEBUG
  void printSoln() {
    for (int i = 0; i < N; i++) {
      for (int j = 0; j < N; j++) {
        cout << soln.at(j + i * N) << " ";
      }
      cout << endl;
    }
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
#endif

private:
  istream_iterator<int> in;
  int N;
  int M;
  vector<bool> G;
#ifdef DEBUG
  vector<bool> soln;
#endif
};

int main() {
  // Pick edges, construct set of at most 2x vertices
  // Use this to bound size of solution subsets examined
  // Thanks Prateek =)

  MinCover mc(cin);
#ifdef DEBUG
  cout << mc << endl;
#endif
  cout << mc.findMin() << endl;
#ifdef DEBUG
  mc.printSoln();
#endif
  return 0;
}
