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

#include <vector>
using std::vector;

class MinCover {
public:
  MinCover() = delete;
  MinCover(istream &is) : in(is), N(*(in++)), M(*(in++)), G(N * N, false) {
    for (int_fast8_t i = 0; i < M; ++i) {
      int_fast8_t v1 = *(in++), v2 = *(in++);
      G.at(v1 + v2 * N) = true;
    }
  }
  MinCover(const MinCover &other) = delete;
  MinCover(MinCover &&other) = delete;
  virtual ~MinCover() = default;
  int findMin() {
    return min(examineVertices(0, 0, G, true), examineVertices(0, 0, G, false));
  }
  int_fast8_t examineVertices(int_fast8_t vertex, int_fast8_t sz,
                              vector<bool> candidate, bool use) {
    // If we've reached the end of our tree or are not using this vertex,
    // move on to the next set of calls
    if (vertex == N || (vertex == N - 1 && !use)) {
      return N;
    } else if (!use) {
      ++vertex;
      return min(examineVertices(vertex, sz, candidate, true),
                 examineVertices(vertex, sz, candidate, false));
    }
    // We are including this vertex, increment the size of the soln
    ++sz;
    // Remove the edges covered by this vertex from the graph
    for (int_fast8_t i = 0; i < N; ++i) {
      candidate.at(i + vertex * N) = false;
      candidate.at(vertex + i * N) = false;
    }
    ++vertex;
    for (auto i : candidate) {
      // As soon as we find a remaining edge, return
      if (i) {
        return min(examineVertices(vertex, sz, candidate, true),
                   examineVertices(vertex, sz, candidate, false));
      }
    }
#ifdef DEBUG
    soln = candidate;
#endif
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
