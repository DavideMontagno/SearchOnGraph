#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <ctime>
#include "./utility/graph.cpp"
#include "./utility/node.cpp"
#include "./utility/utimer.cpp"
#include "./utility/processing_graph.cpp"

using namespace std;
Graph<int> graph;


void BFS(int start_node, int value, int num_nodes, string filename, int wait, int debug)
{
    chrono::microseconds us = 500us; // delay
    int counter_occurrences = 0;
    {
        utimer ut("Time sequential", true);
       
        vector<bool> visited(num_nodes, false);
        queue<Node<int>> curr_frontier;
        queue<Node<int>> next_frontier;
        Node<int> current;
        bool done=false;
        //Take the ROOT node
        curr_frontier.push(graph.getNode(start_node));
        visited[start_node] = true;
        while(!done){
            while (!curr_frontier.empty())
            {
                if(wait) delay(us);
                utimer ut("Time Frontier", debug);
                current = curr_frontier.front();
                {
                    utimer ut("Time counter", debug);
                    if (current.get_value() == value)
                            counter_occurrences++;
                }
                //CHECK EDGES CURRENT NODE
                vector<Edge> child = current.getChild();
                {
                    utimer ut("Time Edges", debug);
                    for (int i = 0; i < child.size(); i++)
                    {
                        int edge_id = child[i].get_edge_id();
                        if (!visited[edge_id])
                        {
                            next_frontier.push(graph.getNode(edge_id));
                            visited[edge_id] = true;
                        }
                    }
                }
                curr_frontier.pop();
            }
            if(next_frontier.empty()) done=true;
            else
            {
                utimer("Time Swap", debug);
                curr_frontier.swap( next_frontier);
            }
        }
        

    }
    cout << "Found values equal to " << value << ": " << counter_occurrences << endl;
}

int main(int argc, char *argv[])
{
    if(argc < 8){
        cout << "Usage: ./seq num_nodes min_edges max_edges start_node value active_wait(0/1) debug(0/1)\n";
        return 0;
    }
    int num_nodes = atoi(argv[1]);
    int min_edges = atoi(argv[2]);
    int max_edges = atoi(argv[3]);
    int start_node = atoi(argv[4]);
    int value = atoi(argv[5]);
    int wait = atoi(argv[6]);
    int debug = atoi(argv[7]);
    cout << "Debug active: " << debug << endl;
    string filename = "graph_generation/data_" + to_string(num_nodes) + "_" + to_string(min_edges) + "_" + to_string(max_edges) + ".txt";
    
    if (start_node>num_nodes)
    {
        std::cout << "Starting node " << start_node << " not exists\n";
        return 0;
    }
    graph = process(filename, num_nodes, start_node);
    BFS(start_node, value, num_nodes,filename,wait,debug);
    return 0;
}