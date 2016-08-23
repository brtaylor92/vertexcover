#include <algorithm>
using std::max_element;
using std::min;
using std::nth_element;

#include <functional>
using std::function;
using std::greater;

#include <iostream>
using std::cin;
using std::cout;

#include <iterator>
using std::distance;
using std::istream_iterator;

#include <numeric>
using std::accumulate;
using std::iota;

#include <vector>
using std::vector;

#include <cstdint>
// int32_t - benchmarking shows this is faster than int16_t or int64_t
// at least on mobile broadwell with clang 3.8

class MinCover {
public:
  MinCover() = delete;

  explicit MinCover(istream_iterator<int32_t> in)
      : N(*in), M(*++in), minSoln(N - 1), G(N * N), deg(N), indices(N) {
    for (int32_t i = 0; i < M; ++i) {
      int32_t v1 = *++in, v2 = *++in;
      G[v1 + v2 * N] = G[v2 + v1 * N] = true;
      ++deg[v1];
      ++deg[v2];
    }
    iota(indices.begin(), indices.end(), 0);
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
      --deg[idx];
      start = next + 1;
    }
  }

  // Profiling indicates that this function is ~1/3 of the runtime
  bool isExposedNode(const int32_t v) {
    buffer.clear();
    const int32_t degV = deg[v];
    const auto first = G.begin() + v * N, end = first + N;
    auto start = first;
    for (int32_t i = 0; i < degV; ++i) {
      const auto next = find(start, end, true);
      const auto idx = distance(first, next);
      if (deg[idx] < degV) {
        return false;
      }
      buffer.push_back(idx);
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
      } else if (degI && isExposedNode(i)) {
        removed += degI;
        for (const auto j : buffer) {
          removeVertex(j);
        }
      }
    }
    return removed;
  }

  int32_t findMinCover(int32_t sz) {
    vector<int32_t> v{-1};
    int32_t bestDeg = 0;

    // Remove cliques/degree one vertices; these must always be in the cover
    int32_t numRemoved = 0;
    do {
      sz += numRemoved;

      if (!M) {
        return minSoln = min(sz, minSoln);
      } else if (sz + 1 >= minSoln) {
        return minSoln;
      }

      // If the best (minSoln - sz - 1) vertices are not sufficient, bound
      const size_t allowed = minSoln - sz - 1;
      nth_element(indices.begin(), indices.begin() + allowed, indices.end(),
                  [&](const auto l, const auto r) { return deg[l] > deg[r]; });
      const int32_t bestPossible =
          accumulate(indices.begin(), indices.begin() + allowed, 0,
                     [&](const int i, const auto x) { return i + deg[x]; });
      if (bestPossible < M) {
        return minSoln;
      }

      const auto maxDeg = max_element(deg.begin(), deg.end());
      v.front() = distance(deg.begin(), maxDeg);
      bestDeg = *maxDeg;
    } while ((numRemoved = removeCliques()));

    // Check if there is better branch taking the neighbors of deg 2 vertices
    for (int32_t i = 0; i < N; ++i) {
      if (deg[i] == 2) {
        const auto start = G.begin() + i * N, end = start + N;
        const auto first = find(start, end, true);
        const auto second = find(first + 1, end, true);
        const int32_t firstIdx = distance(start, first);
        const int32_t secondIdx = distance(start, second);
        const int32_t currentDeg = deg[firstIdx] + deg[secondIdx];

        // Marty's version of the degree 2 propagation; WIP
        /*if (deg[firstIdx] > 2 || deg[secondIdx] > 2) {
          buffer = {firstIdx, secondIdx};
          for (int32_t j = 0; j < static_cast<int32_t>(buffer.size()); ++j) {
            const auto degJ = deg[j];
            const auto firstNeighbor = G.begin() + j * N,
                       last = firstNeighbor + N;
            auto startNeighbor = firstNeighbor;
            for (int32_t n = 0; n < degJ; ++n) {
              auto next = find(startNeighbor, last, true);
              const auto idx = distance(firstNeighbor, next);
              if (deg[idx] == 2 &&
                  find(buffer.begin(), buffer.end(), idx) == buffer.end()) {
                const auto startSearch = G.begin() + idx * N,
                           endSearch = startSearch + N;
                auto firstNewNeighbor = find(startSearch, endSearch, true);
                auto secondNewNeighbor = find(startSearch + 1, endSearch, true);
                auto firstNeighborIdx = distance(startSearch, firstNewNeighbor);
                auto secondNeighborIdx =
                    distance(startSearch, secondNewNeighbor);
                int32_t far = firstNeighborIdx == buffer[j] ? secondNeighborIdx
                                                            : firstNeighborIdx;
                if (far > i &&
                    find(buffer.begin(), buffer.end(), far) == buffer.end()) {
                  buffer.push_back(far);
                  currentDeg += deg[far];
                }
              }
              startNeighbor = next + 1;
            }
          }
        }*/

        // Stuff I already had
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
    deg.swap(oldDeg);
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
        ++sz;
        removeVertex(distance(first, next));
        start = next + 1;
      }
    }

    // Return the size of the best cover found
    return min(likely, findMinCover(sz));
  }

private:
  const int32_t N;
  int32_t M, minSoln;
  vector<bool> G;
  vector<int32_t> deg, indices, buffer;
};

int main() {
  cout << MinCover(istream_iterator<int32_t>(cin)).findMinCover(0) << "\n";
}
