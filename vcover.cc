#include <algorithm>
using std::min;
using std::pop_heap;
using std::push_heap;

#include <functional>
using std::greater;

#include <iostream>
using std::cin;
using std::cout;
using std::istream;

#include <iterator>
using std::istream_iterator;

#include <numeric>
using std::accumulate;

#include <vector>
using std::vector;

#include "Empirical/tools/BitVector.h"
using emp::BitVector;

#include "Empirical/tools/Graph.h"
using emp::Graph;

#include "Empirical/tools/SolveState.h"
using emp::SolveState;

class MinCover {
public:
  MinCover(istream &is) : it(is), N(*it), M(*++it), G(N), soln(N), minSoln(N) {
    for (int32_t i = 0; i < M; ++i) {
      int32_t v1 = *++it, v2 = *++it;
      G.AddEdgePair(v1, v2);
    }
  }
  ~MinCover() = default;
  int32_t getMinSoln() {
    findMinCover();
    return minSoln;
  }
  void findMinCover() {
    bool polyOps = true;
    int32_t v = -1, bestDeg = 0;
    while (polyOps) {
      polyOps = false;

      for (int32_t i = soln.GetNextUnk(-1); i != -1; i = soln.GetNextUnk(i)) {
        // If degree 0, exclude
        if (!G.GetMaskedDegree(i, soln.GetUnkVector())) {
          soln.Exclude(i);
        } else if (formsClique(i)) {
          // If we have a clique, include the neighbors and exclude this node
          soln.IncludeSet(G.GetEdgeSet(i));
          soln.Exclude(i);
          polyOps = true;
        }
      }
      const int32_t in = soln.CountIn();
      if (soln.IsFinal()) {
        minSoln = min(minSoln, in);
        return;
      } else if (in + 1 > minSoln) {
        return;
      }
      // Find the highest degree vertex
      size_t allowed = minSoln - in - 1;
      vector<int32_t> heap;
      heap.reserve(minSoln - in - 1);
      int32_t totalDeg = 0;
      for (int32_t i = soln.GetNextUnk(-1); i != -1; i = soln.GetNextUnk(i)) {
        const int32_t d = G.GetMaskedDegree(i, soln.GetUnkVector());
        totalDeg += d;
        if (d > bestDeg) {
          v = i;
          bestDeg = d;
        }
        if (heap.size() < allowed || (heap.size() && d > heap.at(0))) {
          if (heap.size() < allowed) {
            heap.push_back(d);
          } else {
            pop_heap(begin(heap), end(heap), greater<int32_t>());
            heap.back() = d;
          }
          push_heap(begin(heap), end(heap), greater<int32_t>());
        }
      }
      int32_t max = accumulate(begin(heap), end(heap), 0);
      // If best remaining vertices are larger than the best cover, bound
      if (max < totalDeg / 2) {
        return;
      }
    }
    // Try to beat the highest degree with a pair of neighbors of deg-2 vertex
    vector<int32_t> best(1, v), current;
    for (int32_t i = soln.GetNextUnk(-1); i != -1; i = soln.GetNextUnk(i)) {
      const int32_t d = G.GetMaskedDegree(i, soln.GetUnkVector());
      if (d == 2) {
        const BitVector &n = G.GetEdgeSet(i) & soln.GetUnkVector();
        int32_t v1 = n.FindBit();
        int32_t v2 = n.FindBit(v1 + 1);
        current = {v1, v2};
        int32_t currentDeg = G.GetMaskedDegree(v1, soln.GetUnkVector()) +
                             G.GetMaskedDegree(v2, soln.GetUnkVector());
        for (size_t j = 0; j < current.size(); ++j) {
          const BitVector &e =
              G.GetEdgeSet(current.at(j)) & soln.GetUnkVector();
          for (int32_t k = e.FindBit(); k != -1; k = e.FindBit(++k)) {
            if (G.GetMaskedDegree(k, soln.GetUnkVector()) == 2 &&
                find(begin(current), end(current), k) == end(current)) {
              int32_t far = (G.GetEdgeSet(k) & soln.GetUnkVector()).FindBit();
              if (find(begin(current), end(current), far) == end(current)) {
                current.push_back(far);
                currentDeg += G.GetMaskedDegree(far, soln.GetUnkVector());
              }
            }
          }
        }
        if (currentDeg > bestDeg) {
          best = current;
          bestDeg = currentDeg;
        }
      }
    }
    // Save the solution state before recursing and consider the next cover
    SolveState oldSoln = soln;
    for (const auto i : best) {
      soln.Include(i);
    }
    findMinCover();
    // Restore solution before taking the other branch
    soln = oldSoln;
    // If the "best" choices are not part of the min cover, their neighbors are
    for (const auto i : best) {
      soln.ForceExclude(i);
      soln.IncludeSet(G.GetEdgeSet(i));
    }
    findMinCover();
  }
  bool formsClique(int32_t v) const {
    BitVector n = G.GetEdgeSet(v) & soln.GetUnkVector();
    n[v] = true;
    // Find the neighbors of v which have not yet been evaluated
    for (int32_t i = n.FindBit(); i != -1; i = n.FindBit(++i)) {
      n[i] = false;
      if (!((n & G.GetEdgeSet(i) & soln.GetUnkVector()) == n)) {
        return false;
      }
      n[i] = true;
    }
    // V and its neighbors form a clique
    return true;
  }

private:
  istream_iterator<int32_t> it;
  const int32_t N, M;
  Graph G;
  SolveState soln;
  int32_t minSoln;
};

int main() { cout << MinCover(cin).getMinSoln() << "\n"; }
