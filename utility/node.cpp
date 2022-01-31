#ifndef NODE_CPP
#define NODE_CPP

#include <climits>
#include <iostream>
#include <queue>
#include <vector>
#include "edge.cpp"

template <class T>
class Node
{
    private:            
        int node_id;
        std::vector<Edge> edges;
        T value;

    public:
        Node() {
            this->node_id = -1;
        }
        Node(
            const int &node_id,
            const T &value
        ) {
            this->node_id = node_id;
            this->value = value;
        }
        Node(
            const int &node_id,
            const T &value,
            std::vector<Edge> &edges)
        {
            this->node_id = node_id;
            this->value = value;
            this->edges = edges;
        }
        Node<T>& addChild(const int& node) {
            this->edges.push_back(Edge(node));
            return *this;
        }
        int get_node_id() {
            return this->node_id;
        }
        T get_value() {
            return this->value;
        }
        std::vector<Edge> getChild() {
            return this->edges;
        }
};

#endif