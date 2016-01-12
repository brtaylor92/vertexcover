#include <algorithm>
using std::max_element;
using std::min;

#include <functional>
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

class MinCover {
public:
  MinCover() = delete;

  explicit MinCover(istream_iterator<int32_t> in)
      : N(*in), M(*++in), minSoln(N - 1), G(N * N), deg(N) {
    for (int32_t i = 0; i < M; ++i) {
      int32_t v1 = *++in, v2 = *++in;
      G.at(v1 + v2 * N) = G.at(v2 + v1 * N) = true;
      ++deg.at(v1);
      ++deg.at(v2);
    }
  }

  ~MinCover() = default;

  MinCover(const MinCover &c) = delete;

  MinCover(MinCover &&c) = delete;

  auto getNeighbors(const int32_t v) {
    vector<int32_t> neighbors;
    size_t dv = deg.at(v);
    neighbors.reserve(dv);
    for (int32_t i = 0; i < N && neighbors.size() < dv; ++i) {
      if (G.at(i + v * N)) {
        neighbors.push_back(i);
      }
    }
    return neighbors;
  }

  int32_t removeVertex(int32_t v) {
    for (int i = 0; i < N; ++i) {
      if (G.at(i + v * N)) {
        G.at(i + v * N) = G.at(v + i * N) = false;
        --deg.at(v);
        --deg.at(i);
        --M;
      }
    }
    return 1;
  }

  bool formsClique(int32_t v) const {
    vector<int32_t> n;
    n.reserve(deg.at(v));
    for (int32_t i = 0; i < N; ++i) {
      if (G.at(i + v * N)) {
        n.push_back(i);
      }
    }
    for (auto i : n) {
      if (deg.at(i) < deg.at(v)) {
        return false;
      }
      for (auto j : n) {
        if (i != j && !G.at(j + i * N)) {
          return false;
        }
      }
    }
    return true;
  }

  int32_t removeCliques() {
    int32_t removed = 0;
    for (int32_t i = 0; i < N; ++i) {
      if (deg.at(i) && formsClique(i)) {
        for (int32_t j = 0; j < N; ++j) {
          if (G.at(j + i * N)) {
            removed += removeVertex(j);
          }
        }
      }
    }
    return removed;
  }

  int32_t findMinCover(int32_t sz = 0) {
    vector<int32_t> v(1, -1);
    int32_t bestDeg = 0;

    // Remove cliques/degree one vertices; these must always be in the cover
    int32_t numRemoved = 0;
    do {
      sz += numRemoved;

      if (!M) {
        minSoln = min(sz, minSoln);
        return sz;
      } else if (sz + 1 >= minSoln) {
        return minSoln;
      }

      // TODO(@BRT): Bounding from Marty
      size_t allowed = minSoln - sz - 1;
      vector<int32_t> heap;
      // heap.reserve(allowed);
      for (int32_t i = 0; i < N; ++i) {
        const int32_t localDeg = deg.at(i);
        if (localDeg > bestDeg) {
          v.front() = i;
          bestDeg = localDeg;
        }
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

      int32_t max = accumulate(begin(heap), end(heap), 0);
      int32_t totalDeg = accumulate(begin(deg), end(deg), 0);
      // If the best (minSoln - sz - 1) vertices are not sufficient, bound
      if (max < totalDeg / 2) {
        return minSoln;
      }
      // TODO(@BRT): End bounding from Marty
    } while ((numRemoved = removeCliques()));

    // Check if there is better branch taking the neighbors of deg 2 vertices
    // Right now, this is slower than the version that does not expand. Oops?
    for (int i = 0; i < N; ++i) {
      if (deg.at(i) == 2) {
        auto current = getNeighbors(i);
        int32_t currentDeg = deg.at(current.at(0)) + deg.at(current.at(1));
        if (deg.at(current.at(0)) > 2 || deg.at(current.at(1)) > 2) {
          for (int j = 0; j < static_cast<int32_t>(current.size()); ++j) {
            for (auto k : getNeighbors(current.at(j))) {
              if (deg.at(k) == 2 &&
                  find(current.begin(), current.end(), k) == current.end()) {
                auto n = getNeighbors(k);
                int32_t far = n.at(n.at(0) == current.at(j));
                if (far > i &&
                    find(current.begin(), current.end(), far) ==
                        current.end()) {
                  current.push_back(far);
                  currentDeg += deg.at(far);
                }
              }
            }
          }
          if (currentDeg > bestDeg) {
            v = current;
            bestDeg = currentDeg;
          }
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
      for (int32_t j = 0; j < N; ++j) {
        if (G.at(j + i * N)) {
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
  vector<int32_t> deg;
};

int main() {
  cout << MinCover(istream_iterator<int32_t>(cin)).findMinCover() << "\n";
}
