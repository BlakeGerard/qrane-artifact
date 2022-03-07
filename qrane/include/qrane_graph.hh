/*
Qrane
Filename: qrane_deps.hh
Creation date: December 26, 2020
Copyright (C) 2020

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef QRANE_GRAPH
#define QRANE_GRAPH

#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>

template <class T>
class qrane_graph {

  private:
    bool directed;
    std::unordered_map<T, std::unordered_set<T>> G;
    std::unordered_map<T, unsigned int> in_degree;

  public:
    qrane_graph();
	qrane_graph(bool directed);
    qrane_graph(std::unordered_set<T>& V, bool directed);

    void add_vertex(T v);
    void add_edge(T v, T u);
    void remove_vertex(T v);
    void remove_edge(T v, T u);
    std::unordered_set<T> get_vertex_set();
    std::unordered_set<T> get_frontier();
    std::unordered_set<T> get_neighbors(T v);
    
    typename std::unordered_map<T, std::unordered_set<T>>::const_iterator cbegin() const;
    typename std::unordered_map<T, std::unordered_set<T>>::const_iterator cend() const;
    typename std::unordered_map<T, std::unordered_set<T>>::iterator begin();
    typename std::unordered_map<T, std::unordered_set<T>>::iterator end();

    bool empty();
    unsigned int num_vertices();
    unsigned int num_edges();
    std::string to_string();
    std::string in_degrees_to_string();
    std::string to_python_tuple_list();
    static qrane_graph<T> read_from_edge_list_file(std::string path, bool directed);

    bool contains_vertex(T v);
    bool contains_edge(T v, T u);
    std::unordered_set<T> vertex_pred(T v);
    std::unordered_set<T> vertex_succ(T v);
    std::unordered_set<T> vertex_adj(T v);
    unsigned int unweighted_shortest_path_length(T src, T dst);
};

/*
    Default constructor.
*/
template <class T>
qrane_graph<T>::qrane_graph() {};

/*
    Constructor with directed boolean.
*/
template <class T> 
qrane_graph<T>::qrane_graph(bool directed) {
    this->directed = directed;
};

/*
    Add all vertices in vertex_set to the graph
    with in-degree zero.
*/
template <class T> 
qrane_graph<T>::qrane_graph(std::unordered_set<T>& vertex_set, bool directed) {
    this->directed = directed;

    G.reserve(vertex_set.size());
    for (const T& v : vertex_set) {
        add_vertex(v);
    }
};

/*
    Add vertex v to the graph with in-degree zero.
*/
template <class T> 
void qrane_graph<T>::add_vertex(T v) {
    G.insert(std::make_pair(v, std::unordered_set<T>()));
    in_degree.insert(std::make_pair(v, 0));
};

/*
    Add the edge (v, u) to the graph. Vertex v
    is added to the graph if not already present.
*/
template <class T> 
void qrane_graph<T>::add_edge(T v, T u) {
    if (!G.count(v)) {
        add_vertex(v);
    }
    if (!G.count(u)) {
        add_vertex(u);
    }

    if (!G[v].count(u) && !G[u].count(v)) {
        G[v].insert(u);
        in_degree[u] += 1;
        if (!directed) { 
            G[u].insert(v);
            in_degree[v] += 1; 
        }
    }
};

/*
    Remove vertex v from the graph, if it exists.
*/
template <class T> 
void qrane_graph<T>::remove_vertex(T v) {
    if (G.count(v)) {

        // Decrement in_degree of all neighbors of vertex v
        // Operation is identical for directed and undirected graphs
        for (const T& n : get_neighbors(v)) {
            in_degree[n] -= 1;
        }

        // Remove any reference to v in an adjacency list
        for (auto& entry : G) {
            if (entry.second.count(v)) {
                entry.second.erase(v);
            }
        }

        // Remove v from the graph and in_degree map
        G.erase(v);
        in_degree.erase(v);
    }
};

/*
    Remove edge (v, u) from the graph, if it exists.
*/
template <class T> 
void qrane_graph<T>::remove_edge(T v, T u) {
    if (G.count(v)) {
        if (G[v].count(u)) {
            G[v].erase(u);
            in_degree[u] -= 1;
            if (!directed) {
                G[u].erase(v); 
                in_degree[v] -= 1; 
            }
        }
    }
};

/*
    Get a copy of the vertex set of the graph.
*/
template <class T>
std::unordered_set<T> qrane_graph<T>::get_vertex_set() {
    std::unordered_set<T> vertex_set;
    for (const auto entry : G) {
        vertex_set.insert(entry.first);
    }
    return vertex_set;
};

/*
    Get a copy of the neighboring vertices of
    vertex v. Return an empty std::vector<T> 
    if v is not present in the graph.
*/
template <class T>
std::unordered_set<T> qrane_graph<T>::get_neighbors(T v) {
    if (G.count(v)) {
        return G[v];
    } else {
        return std::unordered_set<T>();
    }
};

/*
    Get the frontier of the graph, that is, the set
    of vertices of zero in-degree.
*/
template <class T>
std::unordered_set<T> qrane_graph<T>::get_frontier() {
    std::unordered_set<T> frontier;
    for (const auto& entry : in_degree) {
        if (entry.second == 0) {
            frontier.insert(entry.first);
        }
    }
    return frontier;
};

template <class T>
unsigned int qrane_graph<T>::num_vertices() {
    return this->G.size();
};

template <class T>
unsigned int qrane_graph<T>::num_edges() {
    unsigned int cnt = 0;
    for (const auto& entry : G) {
        for (const auto& neighbor : G) {
            cnt += 1;
        }
    }
    if (!directed) { cnt /= 2; }
    return cnt;
};

template <class T>
bool qrane_graph<T>::contains_vertex(T v) {
    return this->G.count(v);
};

template <class T>
bool qrane_graph<T>::contains_edge(T v, T u) {
    if (this->G.count(v) && this->G.count(u)) {
        return this->G[v].count(u);
    }
    return false;
};

/*
    Check if the graph is empty.
*/
template <class T>
bool qrane_graph<T>::empty() {
    return G.empty();
};

template <class T>
typename std::unordered_map<T, std::unordered_set<T>>::const_iterator
    qrane_graph<T>::cbegin() const {
    return this->G.cbegin();
};

template <class T>
typename std::unordered_map<T, std::unordered_set<T>>::const_iterator
    qrane_graph<T>::cend() const {
    return this->G.cend();
};

template <class T>
typename std::unordered_map<T, std::unordered_set<T>>::iterator
    qrane_graph<T>::begin() {
    return this->G.begin();
};

template <class T>
typename std::unordered_map<T, std::unordered_set<T>>::iterator
    qrane_graph<T>::end() {
    return this->G.end();
};

/*
    Get a string representation of the adjacency list.
*/
template <class T>
std::string qrane_graph<T>::to_string() {
    std::ostringstream strm;
    strm << "{" << std::endl;
    for (const auto& entry : G) {
        strm << "\t" << entry.first << " -> [ ";
        for (const auto& vertex : entry.second) {
            strm << vertex << " ";
        }
        strm << "]" << std::endl;
    }
    strm << "}" << std::endl;
    return strm.str();
};

/*
    Get a string representation of the adjacency list.
*/
template <class T>
std::string qrane_graph<T>::in_degrees_to_string() {
    std::ostringstream strm;
    strm << "{" << std::endl;
    for (const auto& entry : in_degree) {
        strm << "\t" << entry.first << " -> " << entry.second << std::endl;
    }
    strm << "}" << std::endl;
    return strm.str();
};

/*
    Both QISKIT and TKET accept device coupling graphs
    in the form of a Python list of tuples. In QISKIT's
    case, the edges are considered directed.

    Example: K3
    [[0,1], [1,0], [0,2], [2,0], [1,2], [2,1]]
*/
template <class T>
std::string qrane_graph<T>::to_python_tuple_list() {
    std::ostringstream strm;

    strm << "[";
    for (const auto& entry : G) {
        for (const auto& vertex : entry.second) {
            strm << "[" << entry.first << "," << vertex << "],";
        }
    }
    strm.seekp(-1, std::ios_base::end); // Revert before the trailing ','
    strm << "]";
    return strm.str();
};

/*
    Get the predecessors of a vertex. If the graph
    is undirected, the result of vertex_pred is
    equivalent to vertex_adj.
*/
template <class T>
std::unordered_set<T> qrane_graph<T>::vertex_pred(T v) {
    auto pred = std::unordered_set<T>();
    for (const auto& entry : this->G) {
        for (const auto& neighbor : entry.second) {
            if (neighbor == v) {
                pred.insert(entry.first);
            }
        }
    }
    return pred;
};

/*
    Get the successors of a vertex. If the graph
    is undirected, the result of vertex_succ is
    equivalent to vertex_adj.
*/
template <class T>
std::unordered_set<T> qrane_graph<T>::vertex_succ(T v) {
    if (G.count(v)) {
        return G[v];
    } else {
        return std::unordered_set<T>();
    }
};

/*
    Get the neighbors of a vertex. If the graph
    is directed, the result of vertex_adj is
    equivalent to vertex_succ.
*/
template <class T>
std::unordered_set<T> qrane_graph<T>::vertex_adj(T v) {
    if (G.count(v)) {
        return G[v];
    } else {
        return std::unordered_set<T>();
    }
};

template <class T>
unsigned int qrane_graph<T>::unweighted_shortest_path_length(T src, T dst) {
    auto traversal = std::queue<std::pair<T, unsigned int>>();
    auto explored = std::unordered_set<T>(); 

    traversal.push(std::make_pair(src, 0));
    while (!traversal.empty()) {
        auto front = traversal.front();
        traversal.pop();
        if (explored.count(front.first)) {
            continue;
        }
        if (front.first == dst) {
            return front.second;
        }
        for (const auto& n : vertex_succ(front.first)) {
            traversal.push(std::make_pair(n, front.second + 1));
        }
        explored.insert(front.first);
    }
    return 0;
}

template <class T>
qrane_graph<T> qrane_graph<T>::read_from_edge_list_file(std::string coupling_path, bool directed) {
    qrane_graph<T> ret = qrane_graph<T>(directed);
    std::ifstream in_strm;
    std::string line, lhs, rhs;

    in_strm.open(coupling_path, std::ios_base::in);
    if (in_strm.is_open()) {
        while(std::getline(in_strm, line)) {
            if (line == "") { continue; }
            auto space = line.find(" ");
            lhs = line.substr(0, space);
            rhs = line.substr(space+1);
            auto lhs_id = std::stoi(lhs);
            auto rhs_id = std::stoi(rhs);
            ret.add_edge(lhs_id, rhs_id);
        }
        in_strm.close();
    } else {
        std::cout << "Unable to open coupling graph file. Returning empty qrane_graph.\n";
    }
    return ret;
};

#endif