#include <algorithm>
using std::max_element;
using std::min;
using std::nth_element;

#include <functional>
using std::function;
using std::greater;
using std::reference_wrapper;

#include <iostream>
using std::cin;
using std::cout;

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
      : N(*in), M(*++in), minSoln(N - 1), G(N * N), mayBeExposedNode(N, true),
        deg(N), degRefs(deg.begin(), deg.end()) {
    for (int32_t i = 0; i < M; ++i) {
      const int32_t v1 = *++in, v2 = *++in;
      G[v1 + v2 * N] = G[v2 + v1 * N] = true;
      ++deg[v1];
      ++deg[v2];
    }
    buffer.reserve(M / 2);
  }

  ~MinCover() = default;

  MinCover(const MinCover &c) = delete;

  MinCover(MinCover &&c) = delete;

  void removeVertex(const int32_t v) {
    const auto degV = deg[v];
    deg[v] = 0;
    M -= degV;
    const auto first = G.begin() + v * N, last = first + N;
    auto start = first;
    for (int32_t i = 0; i < degV; ++i) {
      const auto next = find(start, last, true);
      const auto idx = distance(first, next);
      G[v + idx * N] = false;
      mayBeExposedNode[idx] = true;
      --deg[idx];
      start = next + 1;
    }
  }

  bool isExposedNode(const int32_t v) {
    const int32_t degV = deg[v];
    mayBeExposedNode[v] = false;
    const auto first = G.begin() + v * N, end = first + N;
    auto start = first;
    for (int32_t i = 0; i < degV; ++i) {
      const auto next = find(start, end, true);
      const auto idx = distance(first, next);
      if (deg[idx] < degV) {
        return false;
      }
      buffer[i] = idx;
      start = next + 1;
    }
    for (int32_t i = 0; i < degV; ++i) {
      const auto idx = buffer[i];
      for (int32_t j = i + 1; j < degV; ++j) {
        const auto jdx = buffer[j];
        if (idx != jdx && !G[jdx + idx * N]) {
          return false;
        }
      }
    }
    return true;
  }

  int32_t removeCliques() {
    int32_t removed = 0;
    for (int32_t i = 0; i < N; ++i) {
      const auto degI = deg[i];
      if (degI == 1) {
        ++removed;
        const auto start = G.begin() + i * N;
        removeVertex(distance(start, find(start, start + N, true)));
      } else if (mayBeExposedNode[i] && isExposedNode(i)) {
        removed += degI;
        for (int32_t j = 0; j < degI; ++j) {
          removeVertex(buffer[j]);
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

      if (!M && sz < minSoln) {
        return minSoln = sz;
      } else if (sz + 1 >= minSoln) {
        return minSoln;
      }

      // If the best (minSoln - sz - 1) vertices are not sufficient, bound
      const size_t allowed = minSoln - sz - 1;
      nth_element(degRefs.begin(), degRefs.begin() + allowed, degRefs.end(),
                  greater<>());
      const int32_t bestPossible =
          accumulate(degRefs.begin(), degRefs.begin() + allowed, 0);

      if (bestPossible < M) {
        return minSoln;
      }
    } while ((numRemoved = removeCliques()));

    const auto maxDeg = max_element(deg.begin(), deg.end());
    vector<int32_t> v{static_cast<int32_t>(distance(deg.begin(), maxDeg))};
    int32_t bestDeg = *maxDeg;

    // Check if there is better branch taking the neighbors of deg 2 vertex
    for (int32_t i = 0; i < N; ++i) {
      if (deg[i] == 2) {
        const auto start = G.begin() + i * N, end = start + N;
        const auto first = find(start, end, true);
        const auto second = find(first + 1, end, true);
        const int32_t firstIdx = distance(start, first);
        const int32_t secondIdx = distance(start, second);
        const int32_t currentDeg = deg[firstIdx] + deg[secondIdx];
        if (currentDeg > bestDeg) {
          v.assign({firstIdx, secondIdx});
          bestDeg = currentDeg;
        }
      }
    }

    // Save the graph and related information before removing optional
    // vertices
    const int32_t oldM = M;
    auto oldDeg = deg;
    auto oldG = G;

    // Try removing the "best" vertices
    for (const auto i : v) {
      removeVertex(i);
    }
    const int32_t likely = findMinCover(sz + v.size());

    // Restore those vertices before taking the other branch
    M = oldM;
    deg = oldDeg;
    G.swap(oldG);

    // Look at the neighbors of the "best" cover additions for the other
    // branch
    // If the "best" choices are not part of the min cover, their neighbors
    // are
    for (const auto i : v) {
      const auto iDeg = deg[i];
      const auto first = G.begin() + i * N, end = first + N;
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
  const int32_t N;
  int32_t M, minSoln;
  vector<bool> G, mayBeExposedNode;
  vector<int32_t> deg, buffer;
  vector<reference_wrapper<const int32_t>> degRefs;
};

int main() {
  cout << MinCover(istream_iterator<int32_t>(cin)).findMinCover(0) << "\n";
}
