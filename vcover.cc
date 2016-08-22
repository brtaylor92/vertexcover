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
    int32_t vDeg = deg.at(v);
    const auto first = G.begin() + v * N;
    auto start = first, end = first + N;
    for (int32_t i = 0; i < vDeg; ++i) {
      auto next = find(start, end, true);
      buffer.push_back(distance(first, next));
      start = next + 1;
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
        const auto start = G.begin() + i * N;
        removed += removeVertex(distance(start, find(start, start + N, true)));
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
    v.reserve(M / 2);
    int32_t bestDeg = 0;

    // Remove cliques/degree one vertices; these must always be in the cover
    int32_t numRemoved = 0;
    do {
      sz += numRemoved;

      if (!M) {
        return minSoln = min(sz, minSoln);
        ;
      } else if (sz + 1 >= minSoln) {
        return minSoln;
      }

      // Bounding technique added from Marty
      const size_t allowed = minSoln - sz - 1;
      buffer.clear();
      for (int32_t i = 0; i < N; ++i) {
        const int32_t localDeg = deg[i];
        const auto bufSz = buffer.size();
        if (bufSz < allowed || (bufSz && localDeg > buffer.front())) {
          if (bufSz < allowed) {
            buffer.push_back(localDeg);
          } else {
            pop_heap(buffer.begin(), buffer.end(), greater<int32_t>());
            buffer.back() = localDeg;
          }
          push_heap(buffer.begin(), buffer.end(), greater<int32_t>());
        }
      }

      // If the best (minSoln - sz - 1) vertices are not sufficient, bound
      if (accumulate(buffer.begin(), buffer.end(), 0) < M) {
        return minSoln;
      }

      auto maxDeg = std::max_element(deg.begin(), deg.end());
      v.front() = std::distance(deg.begin(), maxDeg);
      bestDeg = *maxDeg;
      // End bounding from Marty
    } while ((numRemoved = removeCliques()));

    // Check if there is better branch taking the neighbors of deg 2 vertices
    int32_t currentDeg = 0;
    for (int32_t i = 0; i < N; ++i) {
      if (deg[i] == 2) {
        const auto start = G.begin() + i * N, end = start + N;
        const auto first = find(start, end, true);
        const auto second = find(first + 1, end, true);
        const int32_t firstIdx = distance(start, first);
        const int32_t secondIdx = distance(start, second);
        currentDeg = deg[firstIdx] + deg[secondIdx];

        if (currentDeg > bestDeg) {
          v = {firstIdx, secondIdx};
          bestDeg = currentDeg;
        }
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
      const auto iDeg = deg[i];
      auto first = G.begin() + i * N, start = first;
      const auto end = first + N;
      for (int32_t j = 0; j < iDeg; ++j) {
        auto next = find(start, end, true);
        sz += removeVertex(distance(first, next));
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
  vector<int32_t> deg, buffer;
  // static const function<bool(bool)> exists;
};

// const function<bool(bool)> MinCover::exists{[](const bool x) { return x; }};

int main() {
  cout << MinCover(istream_iterator<int32_t>(cin)).findMinCover(0) << "\n";
}
