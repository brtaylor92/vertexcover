#include <cmath>
using std::exp2;

#include <iostream>
using std::cin;
using std::cout;
using std::endl;

#include <iterator>
using std::istream_iterator;

#include <numeric>
using std::accumulate;

#include <set>
using std::set;

#include <vector>
using std::vector;

int main() {
  // Read input
  istream_iterator<int> in(cin);
  int N = *(in++), M = *(in++);

  // Pick edges, construct set of at most 2x vertices
  // Use this to bound size of solution subsets examined
  // Thanks Prateek =)
  // set<int> S;

  // Construct adjacency matrix
  vector<bool> G(N * N, false);
  // All vertices have a "call" with themselves
  for (int i = 0; i < N; i++) {
    G.at(i * N + i) = true;
  }
  for (int i = 0; i < M; i++) {
    int v1 = *(in++), v2 = *(in++);
    G.at(v2 + v1 * N) = true;
    G.at(v1 + v2 * N) = true;
    // S.insert(v1);
    // S.insert(v2);
  }

  // cout << "Max soln: " << S.size() << endl;
  // cout << "Min soln: " << S.size()/2 << endl;

  // Print adjacency matrix
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      cout << G.at(j + i * N) << " ";
    }
    cout << endl;
  }

  // Woo, I don't know how to look at all possible subsets
  // Seriously, who thought letting me write this was a good idea?
  vector<vector<bool>> possible(exp2(N), vector<bool>(N, false));
  for (int i = 1; i <= N; i++) {
    if (/*subset of this size covers*/ false ) {
      cout << i << endl;
      return 0;
    }
  }
  return 1;
}
