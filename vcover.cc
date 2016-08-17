#include <algorithm>
using std::max_element;
using std::min;

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

#include <vector>
using std::vector;

#include <cstdint>
// int32_t - benchmarking shows this is faster than int16_t or int64_t
// at least on mobile broadwell with clang 3.8

class MinCover {
public:
  MinCover() = delete;

  explicit MinCover(istream_iterator<int32_t> in)
      : N(*in), M(*++in), minSoln(N - 1), G(N * N), deg(N) {
    for (int32_t i = 0; i < M; ++i) {
      int32_t v1 = *++in, v2 = *++in;
      G[v1 + v2 * N] = G[v2 + v1 * N] = true;
      ++deg[v1];
      ++deg[v2];
    }
    buffer.reserve(M / 2);
  }

  ~MinCover() = default;

  MinCover(const MinCover &c) = default;

  MinCover(MinCover &&c) = default;

  int32_t removeVertex(int32_t v) {
    for (int32_t i = 0; i < N; ++i) {
      if (G[i + v * N]) {
        G[i + v * N] = G[v + i * N] = false;
        --deg[v];
        --deg[i];
        --M;
      }
    }
    return 1;
  }

  // Profiling indicates that this function is ~1/3 of the runtime
  // Much of that is spent in operator new - allocating n?
  bool formsClique(int32_t v) {
    buffer.clear();
    for (int32_t i = 0; i < N; ++i) {
      if (G[i + v * N]) {
        buffer.push_back(i);
      }
    }
    const int32_t sz = buffer.size();
    for (int32_t i = 0; i < sz; ++i) {
      const auto idx = buffer.at(i);
      if (deg[idx] < deg[v]) {
        return false;
      }
      for (int32_t j = i + 1; j < sz; ++j) {
        const auto jdx = buffer.at(j);
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
      if (deg[i] == 1) {
        for (int32_t j = 0; j < N; ++j) {
          if (G[j + i * N]) {
            removed += removeVertex(j);
            break;
          }
        }
      } else if (deg[i] && formsClique(i)) {
        for (auto j : buffer) {
          removed += removeVertex(j);
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
        minSoln = min(sz, minSoln);
        return minSoln;
      } else if (sz + 1 >= minSoln) {
        return minSoln;
      }

      // Bounding technique added from Marty
      const size_t allowed = minSoln - sz - 1;
      vector<int32_t> heap;
      heap.reserve(allowed);
      for (int32_t i = 0; i < N; ++i) {
        const int32_t localDeg = deg[i];
        if (heap.size() < allowed ||
            (!heap.empty() && localDeg > heap.front())) {
          if (heap.size() < allowed) {
            heap.push_back(localDeg);
          } else {
            pop_heap(begin(heap), end(heap), greater<int32_t>());
            heap.back() = localDeg;
          }
          push_heap(begin(heap), end(heap), greater<int32_t>());
        }
      }

      // If the best (minSoln - sz - 1) vertices are not sufficient, bound
      if (accumulate(heap.begin(), heap.end(), 0) < M) {
        return minSoln;
      }
      auto maxDeg = std::max_element(deg.begin(), deg.end());
      v.front() = std::distance(deg.begin(), maxDeg);
      bestDeg = *maxDeg;
      // End bounding from Marty
    } while ((numRemoved = removeCliques()));

    // Check if there is better branch taking the neighbors of deg 2 vertices
    for (int32_t i = 0; i < N; ++i) {
      if (deg[i] == 2) {
        vector<int32_t> group;
        int32_t currentDeg = 0;
        for (int32_t j = 0; j < N; ++j) {
          if (G[j + i * N]) {
            group.push_back(j);
            currentDeg += deg[j];
          }
        }

        // Marty's version
        /*if (deg[group[0]] > 2 || deg[group[1]] > 2) {
          for (int32_t j = 0; j < static_cast<int32_t>(group.size()); ++j) {
            buffer.clear();
            for (int32_t n = 0; n < N; ++n) {
              if (G[n + j * N]) {
                buffer.push_back(n);
              }
            }
            for (auto n : buffer) {
              if (deg[n] == 2 &&
                  find(group.begin(), group.end(), n) == group.end()) {
                const auto firstNeighborIt =
                    find_if(G.begin() + n * N, G.begin() + (n + 1) * N, exists);
                const int32_t firstNeighbor =
                    distance(G.begin() + n * N, firstNeighborIt);
                const auto secondNeighborIt = find_if(
                    firstNeighborIt + 1, G.begin() + (n + 1) * N, exists);
                const int32_t secondNeighbor =
                    distance(G.begin() + n * N, secondNeighborIt);
                const int32_t far =
                    firstNeighbor == group[n] ? secondNeighbor : firstNeighbor;
                if (far > i &&
                    find(group.begin(), group.end(), far) == group.end()) {
                  group.push_back(far);
                }
              }
            }
          }*/

          // Slowdown from 6s to 25s???
          /*currentDeg = accumulate(v.begin(), v.end(), 0,
                                 [&](const int32_t init, const auto x) {
                                   return init + deg[x];
                                 });*/
          // Stuff I already had
          if (currentDeg > bestDeg) {
            v.swap(group);
            bestDeg = currentDeg;
          }
        //}
      }
    }

    // Save the graph and related information before removing optional
    // vertices
    int32_t oldM = M;
    auto oldDeg = deg;
    auto oldG = G;

    // Try removing the "best" vertices
    for (auto i : v) {
      removeVertex(i);
    }
    int32_t likely = findMinCover(sz + v.size());

    // Restore those vertices before taking the other branch
    M = oldM;
    deg.swap(oldDeg);
    G.swap(oldG);

    // Look at the neighbors of the "best" cover additions for the other
    // branch
    // If the "best" choices are not part of the min cover, their neighbors
    // are
    for (auto i : v) {
      for (int32_t j = 0; j < N; ++j) {
        if (G[j + i * N]) {
          sz += removeVertex(j);
        }
      }
    }

    // Return the size of the best cover found
    return min(likely, findMinCover(sz));
  }

private:
  const int32_t N;
  int32_t M, minSoln;
  vector<bool> G;
  vector<int32_t> deg, buffer;
  static const function<bool(bool)> exists;
};

const function<bool(bool)> MinCover::exists{[](const bool x) { return x; }};

int main() {
  cout << MinCover(istream_iterator<int32_t>(cin)).findMinCover(0) << "\n";
}
