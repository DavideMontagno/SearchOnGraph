#include <string>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <ctime>
using namespace std;

Graph<int> process(string filename, int num_nodes, int start_node){
    Graph<int> graph;
    {
        utimer ut("Graph processing", true);
        ifstream file(filename);
        int nodeCount;
        file >> nodeCount;
        
        for (int i = 0; i < nodeCount; ++i) {
            int nodeID, val;
            file >> nodeID >> val;
            graph.addNode(nodeID, val);
        }
        while( file.peek() != EOF ) {
            int src, dest;
            file >> src >> dest;
            graph.addEdge(src, dest);
            graph.addEdge(dest, src);
        }
    }
    return graph;
}

