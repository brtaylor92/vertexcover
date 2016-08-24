#include <algorithm>
using std::max_element;
using std::min;
using std::nth_element;

#include <functional>
using std::greater;
using std::reference_wrapper;

#include <iostream>
using std::cin;
using std::cout;
using std::endl;

#include <iterator>
using std::distance;
using std::istream_iterator;

#include <numeric>
using std::accumulate;

#include <vector>
using std::vector;

#include <cstdint>
// int32_t - benchmarking shows this is faster than int16_t or int64_t
// at least on mobile broadwell with clang 3.8

class MinCover {
public:
  MinCover() = delete;

  explicit MinCover(istream_iterator<int32_t> in)
      : numVertices(*in), numEdges(*++in), minSoln(numVertices - 1),
        G(numVertices * numVertices), mayBeExposedVertex(numVertices, true),
        degrees(numVertices), degreeRefs(degrees.cbegin(), degrees.cend()) {
    for (int32_t i = 0; i < numEdges; ++i) {
      const int32_t v1 = *++in, v2 = *++in;
      G[v1 + v2 * numVertices] = G[v2 + v1 * numVertices] = true;
      ++degrees[v1];
      ++degrees[v2];
    }
    const int32_t maxDeg = *max_element(degrees.cbegin(), degrees.cend());
    subGraph.reserve(maxDeg);
  }

  ~MinCover() = default;

  MinCover(const MinCover &c) = delete;

  MinCover(MinCover &&c) = delete;

  void removeVertex(const int32_t v) {
    const auto degV = degrees[v];
    degrees[v] = 0; // This suffices rather than setting all edges to false
    numEdges -= degV;
    // Set all edges from neighbors to v to false
    const auto first = G.cbegin() + v * numVertices, last = first + numVertices;
    auto start = first;
    for (int32_t i = 0; i < degV; ++i) {
      const auto next = find(start, last, true);
      const auto idx = distance(first, next);
      G[v + idx * numVertices] = false;
      mayBeExposedVertex[idx] = true;
      --degrees[idx];
      start = next + 1;
    }
  }

  bool isExposedVertex(const int32_t v) {
    // This node either is not exposed or all neighbors will be removed
    mayBeExposedVertex[v] = false;
    const int32_t degV = degrees[v];
    const auto first = G.cbegin() + v * numVertices, end = first + numVertices;
    auto start = first;
    for (int32_t i = 0; i < degV; ++i) {
      const auto next = find(start, end, true);
      const auto idx = distance(first, next);
      if (degrees[idx] < degV) {
        // If the neighbor's degree is less than this node's,
        // this subgraph is not a clique
        return false;
      }
      subGraph[i] = idx;
      start = next + 1;
    }
    // Now check if the subgraph is complete (forms a clique)
    for (int32_t i = 0; i < degV; ++i) {
      const auto idx = subGraph[i];
      for (int32_t j = i + 1; j < degV; ++j) {
        const auto jdx = subGraph[j];
        if (idx != jdx && !G[jdx + idx * numVertices]) {
          // If there is no edge between any 2 neighbors,
          // this subgraph is not a clique
          return false;
        }
      }
    }
    return true;
  }

  int32_t removeCliques() {
    int32_t removed = 0;
    for (int32_t i = 0; i < numVertices; ++i) {
      const auto degI = degrees[i];
      if (degI == 1) {
        // If this vertex has degree 1, its neighbor must be included
        ++removed;
        const auto start = G.cbegin() + i * numVertices;
        removeVertex(distance(start, find(start, start + numVertices, true)));
      } else if (mayBeExposedVertex[i] && isExposedVertex(i)) {
        // If this is an exposed node of a clique
        // (the subgraph composed of this node and its neighbors is a clique)
        // then include all its neighbors
        removed += degI;
        for (int32_t j = 0; j < degI; ++j) {
          removeVertex(subGraph[j]);
        }
      }
    }
    return removed;
  }

  int32_t findMinCover(int32_t sz) {
    // Remove cliques/degree one vertices; these must always be in the cover
    int32_t numRemoved = 0;
    do {
      sz += numRemoved;

      // If there are no edges remaining and this cover is smaller than the
      // previous best update the best cover and return
      if (!numEdges && sz < minSoln) {
        return minSoln = sz;
      } else if (sz + 1 >= minSoln) {
        // If no better cover is possible on this branch, bound
        return minSoln;
      }

      // If the best (minSoln - sz - 1) vertices are not sufficient, bound
      const size_t allowed = minSoln - sz - 1;
      nth_element(degreeRefs.begin(), degreeRefs.begin() + allowed,
                  degreeRefs.end(), greater<>());
      const int32_t bestPossible =
          accumulate(degreeRefs.cbegin(), degreeRefs.cbegin() + allowed, 0);

      if (bestPossible < numEdges) {
        return minSoln;
      }
    } while ((numRemoved = removeCliques()));

    // Take the greedy choice (highest degree vertex)
    const auto maxDeg = max_element(degrees.cbegin(), degrees.cend());
    vector<int32_t> v{static_cast<int32_t>(distance(degrees.cbegin(), maxDeg))};
    int32_t bestDeg = *maxDeg;

    // Check if there is higher total degree taking the neighbors of a deg 2
    // vertex
    for (int32_t i = 0; i < numVertices; ++i) {
      if (degrees[i] == 2) {
        const auto start = G.cbegin() + i * numVertices,
                   end = start + numVertices;
        const auto first = find(start, end, true);
        const auto second = find(first + 1, end, true);
        const int32_t firstIdx = distance(start, first);
        const int32_t secondIdx = distance(start, second);
        const int32_t currentDeg = degrees[firstIdx] + degrees[secondIdx];
        if (currentDeg > bestDeg) {
          v.assign({firstIdx, secondIdx});
          bestDeg = currentDeg;
        }
      }
    }

    // Save the graph and related information before removing likely vertices
    const int32_t oldNumEdges = numEdges;
    const auto oldDegrees = degrees;
    auto oldG = G;

    // Try removing the "best" vertices
    for (const auto i : v) {
      removeVertex(i);
    }
    const int32_t likely = findMinCover(sz + v.size());

    // Restore those vertices before taking the other branch
    numEdges = oldNumEdges;
    degrees = oldDegrees;
    G.swap(oldG);

    // If the "best" choices are not in the min cover, their neighbors are
    for (const auto i : v) {
      const auto iDeg = degrees[i];
      const auto first = G.cbegin() + i * numVertices,
                 end = first + numVertices;
      auto start = first;
      for (int32_t j = 0; j < iDeg; ++j) {
        const auto next = find(start, end, true);
        removeVertex(distance(first, next));
        start = next + 1;
        ++sz;
      }
    }

    // Return the size of the best cover found
    return min(likely, findMinCover(sz));
  }

private:
  const int32_t numVertices;
  int32_t numEdges, minSoln;
  vector<bool> G, mayBeExposedVertex;
  vector<int32_t> degrees, subGraph;
  vector<reference_wrapper<const int32_t>> degreeRefs;
};

int main() {
  // Using \n here is measurably faster than a std::endl
  // on the order of 0.02s
  cout << MinCover(istream_iterator<int32_t>(cin)).findMinCover(0) << "\n";
}
