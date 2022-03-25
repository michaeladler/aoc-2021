#include <cstdint>
#include <sstream>
#include <iostream>
#include <cassert>
#include <span>

namespace graph {

/*
Based on:

Copyright 2003 by Steven S. Skiena; all rights reserved.

Permission is granted for use in non-commerical applications
provided this copyright notice remains intact and unchanged.

This program appears in my book:

"Programming Challenges: The Programming Contest Training Manual"
by Steven Skiena and Miguel Revilla, Springer-Verlag, New York 2003.

See our website www.programming-challenges.com for additional information.

This book can be ordered from Amazon.com at

http://www.amazon.com/exec/obidos/ASIN/0387001638/thealgorithmrepo/

*/
template <uint8_t MAXV = 100, uint8_t MAXDEGREE = 50, typename T = uint8_t>
class Graph {
   private:
    T m_edges[MAXV + 1][MAXDEGREE]; /* adjacency info */
    T m_degree[MAXV + 1];           /* outdegree of each vertex */
    T m_nvertices;                  /* number of vertices in the graph */
    T m_nedges;                     /* number of edges in the graph */
   public:
    Graph() : m_nvertices(0), m_nedges(0) {
        for (auto i = 1; i <= MAXV; i++) m_degree[i] = 0;
    }

    T nvertices() const {
        return m_nvertices;
    }

    T nedges() const {
        return m_nedges;
    }

    T degree(T x) const {
        assert(x > 0);
        assert(x <= MAXV);
        return m_degree[x];
    }

    std::span<const T> edges(T x) const {
        assert(x > 0);
        assert(x <= MAXV);
        return std::span(m_edges[x], m_degree[x]);
    }

    void insert_edge(T x, T y, bool directed) {
        assert(x > 0);
        assert(y > 0);
        assert(m_degree[x] <= MAXDEGREE);

        m_nvertices = std::max(x, y);

        m_edges[x][m_degree[x]] = y;
        m_degree[x]++;

        if (!directed)
            insert_edge(y, x, true);
        else
            m_nedges++;
    }

    bool delete_edge(int x, int y, bool directed) {
        assert(x > 0);
        assert(y > 0);
        for (T i = 0; i < m_degree[x]; i++)
            if (m_edges[x][i] == y) {
                m_degree[x]--;
                m_edges[x][i] = m_edges[x][m_degree[x]];

                if (!directed)
                    (void)delete_edge(y, x, true);

                return true;
            }

        return false;
    }

    friend std::ostream& operator<<(std::ostream& os, const Graph& g) {
        for (T i = 1; i <= g.m_nvertices; i++) {
            os << +i << " :";
            for (T j = 0; j < g.m_degree[i]; j++)
                os << " " << +g.m_edges[i][j];
            os << std::endl;
        }
        return os;
    }
};

}  // namespace graph
