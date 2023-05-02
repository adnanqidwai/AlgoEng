#include <iostream>
#include <vector>
#include <utility>
#include "boost/graph/adjacency_list.hpp"
#include "boost/graph/random.hpp"
#include "boost/graph/kruskal_min_spanning_tree.hpp"
#include "property_map.hpp"

using namespace boost;

typedef adjacency_list<vecS, vecS, undirectedS, no_property, property<edge_weight_t, double>> Graph;
typedef graph_traits<Graph>::edge_descriptor Edge;
typedef graph_traits<Graph>::vertex_descriptor Vertex;

Graph k_spanner(const Graph &G, int k)
{
    int n = num_vertices(G);
    int m = num_edges(G);
    int r = static_cast<int>(ceil(log(n) / log(k)));

    // Randomly partition vertices into k^2 groups
    std::vector<int> group(n);
    int cnt = 0;
    for (int i = 0; i < r; ++i)
    {
        std::vector<int> perm(n);
        std::generate(perm.begin(), perm.end(), [&cnt]()
                      { return cnt++; });
        random_shuffle(perm.begin(), perm.end());
        int g = 0;
        for (int j = 0; j < n; j += n / (k * k))
        {
            for (int l = j; l < j + n / (k * k); ++l)
            {
                group[perm[l]] = g;
            }
            ++g;
        }
    }

    // Compute k-spanner
    Graph H(n);
    property_map<Graph, edge_weight_t>::type weight = get(edge_weight, G);
    for (int i = 0; i < r; ++i)
    {
        std::vector<Graph> groups(k * k);
        for (int j = 0; j < m; ++j)
        {
            Vertex u = source(edge(j, G), G);
            Vertex v = target(edge(j, G), G);
            int gu = group[u];
            int gv = group[v];
            if (gu != gv)
            {
                double w = get(weight, j);
                int idx = gu * k + gv;
                if (w < get(weight, edge(groups[idx].edge(u, v).first, groups[idx]).second))
                {
                    Edge e;
                    bool inserted;
                    tie(e, inserted) = add_edge(u, v, groups[idx]);
                    put(weight, e, w);
                }
            }
        }
        for (int j = 0; j < k * k; ++j)
        {
            Graph &Gj = groups[j];
            std::vector<Edge> tree;
            kruskal_minimum_spanning_tree(Gj, std::back_inserter(tree));
            for (const auto &e : tree)
            {
                int u = source(e, Gj);
                int v = target(e, Gj);
                add_edge(u, v, get(weight, e), H);
            }
        }
    }

    return H;
}

int main()
{
    // Example graph
    Graph G(5);
    add_edge(0, 1, 1.0, G);
    add_edge(0, 2, 2.0, G);
    add_edge(1, 2, 3.0, G);
    add_edge(1, 3, 4.0, G);
    add_edge(2, 3, 5.0, G);
    add_edge(3, 4, 6.0, G);

    std::cout << "Original graph:\n";
    graph_traits<Graph>::edge_iterator ei, ei_end;
    for (tie(ei, ei_end) = edges(G); ei != ei_end; ++ei)
    {
        Vertex u = source(*ei, G);
        Vertex v = target(*ei, G);
        std::cout << u << " -- " << v << " : " << get(get(edge_weight, G), *ei) << "\n";
    }
    std::cout << "\n";

    int k = 2;
    Graph H = k_spanner(G, k);

    std::cout << "k-spanner with k = " << k << ":\n";
    for (tie(ei, ei_end) = edges(H); ei != ei_end; ++ei)
    {
        Vertex u = source(*ei, H);
        Vertex v = target(*ei, H);
        std::cout << u << " -- " << v << " : " << get(get(edge_weight, H), *ei) << "\n";
    }
    std::cout << "\n";

    return 0;
}
