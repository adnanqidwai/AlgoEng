#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <limits>

using namespace std;

typedef pair<int, int> Edge;
typedef pair<double, int> WeightedEdge;
typedef vector<Edge> Graph;
typedef vector<WeightedEdge> WeightedGraph;

vector<double> dijkstra(const WeightedGraph& G, int s) {
    int n = G.size();
    vector<double> dist(n, numeric_limits<double>::infinity());
    vector<bool> visited(n, false);
    priority_queue<WeightedEdge, vector<WeightedEdge>, greater<WeightedEdge>> Q;
    dist[s] = 0;
    Q.push(make_pair(0, s));
    while (!Q.empty()) {
        int u = Q.top().second;
        Q.pop();
        if (visited[u]) continue;
        visited[u] = true;
        for (auto& e : G[u]) {
            int v = e.second;
            double w = e.first;
            if (dist[v] > dist[u] + w) {
                dist[v] = dist[u] + w;
                Q.push(make_pair(dist[v], v));
            }
        }
    }
    return dist;
}

Graph k_spanner(const WeightedGraph& G, int k) {
    int n = G.size();
    Graph H;
    for (int u = 0; u < n; u++) {
        for (auto& e : G[u]) {
            int v = e.second;
            double w = e.first;
            vector<double> dist_u = dijkstra(G, u);
            vector<double> dist_v = dijkstra(G, v);
            double dist_uv = dist_u[v];
            if (dist_uv <= k * w) {
                H.push_back(make_pair(u, v));
            }
        }
    }
    return H;
}

int main() {
    WeightedGraph G = {
        {1.0, 1},
        {2.0, 2},
        {3.0, 3},
        {4.0, 4},
        {5.0, 0},
        {6.0, 1},
        {7.0, 2},
        {8.0, 3},
        {9.0, 4}
    };

    int k = 2;
    Graph H = k_spanner(G, k);

    cout << "k-spanner with k = " << k << ":\n";
    for (auto& e : H) {
        int u = e.first;
        int v = e.second;
        cout << u << " -- " << v << "\n";
    }
    cout << "\n";

    return 0;
}
