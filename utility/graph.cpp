#ifndef GRAPH_CPP
#define GRAPH_CPP

#include <iostream>
#include <vector>
#include <stdlib.h>

#include "edge.cpp"
#include "node.cpp"

template <class NT>
class Graph {
    private:
        std::vector<Node<NT>> nodes;
    
    public:
        Graph() {}
        Graph(const std::vector<Node<NT>>& nodes) {
            this->nodes = nodes;
        }
        Graph& addNode(const Node<NT>& node) {
            this->nodes.push_back(node);
            return *this;
        }
        Graph &addNode(int &node_id, const NT &data)
        {
            this->nodes.push_back(Node<NT>(node_id, data));
            return *this;
        }
        Graph &addNode(const NT &data)
        {
            this->nodes.push_back(Node<NT>(this->nodes.size(), data));
            return *this;
        }
        Graph& addEdge(const int& node_id, const Edge& edge){
            if(node_id >= this->nodes.size()) {
                std::cout << "Node " << node_id << " is not in the Graph1" << std::endl;
                exit(EXIT_FAILURE);
            }
            this->nodes[node_id].edges.push_back(edge);
            return *this;
        }
        Graph &addEdge(const int &node_id, const int &edge)
        {
            if (node_id >= this->nodes.size())
            {
                std::cout << "Node " << node_id << " is not in the Graph2" << std::endl;
                exit(EXIT_FAILURE);
            }
            this->nodes[node_id].addChild(edge);
            return *this;
        }
        Node<NT> getNode(const int& node_id)
        {
            if (node_id >= this->nodes.size())
            {
                std::cout << "Node " << node_id << " is not in the Graph3" << std::endl;
                exit(EXIT_FAILURE);
            }
            return this->nodes[node_id];
        }
        int getNodeCounts() {
            return this->nodes.size();
        }
};

#endif
