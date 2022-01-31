#include <ctime>
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <bits/stdc++.h>
#include <time.h>
using namespace std;

int main(int argc, char **argv)
{
    if(argc < 6) {
        cout << "Usage: ./gen_graph total_nodes min_edges max_edges max_value seed" << std::endl;
        return 0;
    }
    int total_nodes = atoi(argv[1]); 
    int min_edges = atoi(argv[2]); 
    int max_edges = atoi(argv[3]); 
    int max_value = atoi(argv[4]); 
    int seed = atoi(argv[5]);
    srand(seed);
    
    auto filename = "data_" + to_string(total_nodes) + "_" + to_string(min_edges) + "_" + to_string(max_edges) + ".txt";
    vector<int> indices; 

    //CREATE THE FILE
    ofstream graph(filename);

    graph << total_nodes << "\n";
    for (int i = 0; i < total_nodes; ++i) {
        // We insert in each line one node id and a random value took randomly in the interval pre-inserted (its corresponding value)
        graph << (i) << " " << (rand() % max_value + 1) << '\n'; 
    }
    int id;
    for (int i = 0; i < total_nodes; ++i) {

        // Each node has a number of edge equal to a random process defined in the interval min max.
        int num_edges = min_edges + rand() % (( max_edges + 1 ) - min_edges);
        cout << "Adding to " << i << " number of edges: " << num_edges << "\n";
        indices = {i};
        while(num_edges > 0){
            id = rand() % total_nodes; 
            // We insert a line for each edge (no duplicates or the insertion of i)
            if((find(indices.begin(), indices.end(), id) == indices.end())){
                indices.push_back(id);
                //adding to edge i a random node id
                graph << (i) << " " << (id) << '\n'; 
                num_edges--;
               
            }
        }
    }
    graph.close();
    return 0;
}
