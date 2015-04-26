#include <algorithm>
using std::min;

#include <iostream>
using std::cin;
using std::cout;
using std::istream;

#include <iterator>
using std::istream_iterator;

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
  MinCover(istream &is)
      : in(is), N(*in), M(*++in), minSoln(N - 1), G(N), soln(N) {
    for (int32_t i = 0; i < M; ++i) {
      int32_t v1 = *++in, v2 = *++in;
      G.AddEdgePair(v1, v2);
    }
  }
  ~MinCover() = default;
  int32_t getMinSoln() {
    findCover();
    return minSoln;
  }
  void findCover() {
    removeCliques();
    // Check for completion
    if (soln.IsFinal()) {
      minSoln = min(minSoln, soln.CountIn());
      return;
    }
    // Find the highest degree vertex or pair of deg-2 neighbors
    vector<int32_t> best(1, -1);
    int32_t bestDeg = 0;
    int32_t totalDeg = 0;
    for (int32_t i = soln.GetNextUnk(-1); i != -1; i = soln.GetNextUnk(i)) {
      const int32_t d = G.GetMaskedDegree(i, soln.GetUnkVector());
      totalDeg += d;
      if (d > bestDeg) {
        best = {i};
        bestDeg = d;
      }
    }
    // If M/d(v) is larger than the best cover, bound
    if (soln.CountIn() + (totalDeg - 2) / (bestDeg * 2) + 1 >= minSoln) {
      return;
    }
    vector<int32_t> current;
    current.reserve(2);
    int32_t currentDeg = 0;
    for (int32_t i = soln.GetNextUnk(-1); i != -1; i = soln.GetNextUnk(i)) {
      const int32_t d = G.GetMaskedDegree(i, soln.GetUnkVector());
      if (d == 2) {
        current.clear();
        currentDeg = 0;
        const BitVector &e = G.GetEdgeSet(i) & soln.GetUnkVector();
        for (int32_t j = e.FindBit(); j != -1; j = e.FindBit(++j)) {
          current.push_back(j);
          currentDeg += G.GetMaskedDegree(j, soln.GetUnkVector());
        }
        if (currentDeg > bestDeg) {
          best = current;
          bestDeg = currentDeg;
        }
      }
    }
    // Save the solution state before removing optional vertices
    SolveState oldSoln = soln;
    // Consider the next choice of cover
    for (const auto i : best) {
      soln.Include(i);
    }
    findCover();
    // Restore vertices before taking the other branch
    soln = oldSoln;
    // Look at the neighbors of the "best" cover additions for the other branch
    // If the "best" choices are not part of the min cover, their neighbors are
    for (const auto i : best) {
      soln.ForceExclude(i);
      soln.IncludeSet(G.GetEdgeSet(i));
    }
    findCover();
  }
  bool formsClique(int32_t v) {
    BitVector n = G.GetEdgeSet(v) & soln.GetUnkVector();
    n[v] = true;
    // Find the neighbors of v which have not yet been evaluated
    for (int32_t i = n.FindBit(); i != -1; i = n.FindBit(++i)) {
      BitVector n2 = G.GetEdgeSet(i) & soln.GetUnkVector();
      n[i] = false;
      if (!((n & n2) == n)) {
        return false;
      }
      n[i] = true;
    }
    // V and its neighbors form a clique
    return true;
  }
  void removeCliques() {
    for (int32_t i = soln.GetNextUnk(-1); i != -1; i = soln.GetNextUnk(i)) {
      // If degree 0, exclude
      if (!G.GetMaskedDegree(i, soln.GetUnkVector())) {
        soln.Exclude(i);
      } else if (formsClique(i)) {
        // If we have a clique, include the neighbors and exclude this node
        soln.IncludeSet(G.GetEdgeSet(i));
        soln.Exclude(i);
        i = -1;
      }
    }
  }

private:
  istream_iterator<int32_t> in;
  const int32_t N, M;
  int32_t minSoln;
  Graph G;
  SolveState soln;
};

int main() { cout << MinCover(cin).getMinSoln() << "\n"; }
