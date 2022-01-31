#include <iostream>
#include <list>
#include <algorithm>
#include <string>
#include <vector>
#include <queue>
#include <chrono>
#include <mutex>
#include <thread>
#include <ctime>   // For time()
#include <cstdlib> // For srand() and rand()
#include <ff/ff.hpp>
#include <ff/node.hpp>
#include "./utility/node.cpp"
#include "./utility/graph.cpp"
#include "./utility/utimer.cpp"
#include "./utility/barrier.cpp"
#include "./utility/processing_graph.cpp"
#include <ff/ff.hpp>
#include <ff/node.hpp>

using namespace ff;
using namespace std;


std::vector<std::mutex> global_queue_mutex;
std::atomic<int> global_counter = 0;
int value;
int start_node;
int num_workers;
int num_nodes;
int min_edges;
int max_edges;
int wait;
int debug;
int steal;
Graph<int> graph;
std::vector<bool> visited;
std::vector<std::mutex>  visited_mutex;
vector<queue<Node<int>>> current_frontier;
MyBarrier barrier(0);

void BFS(int id){
    bool stop=false;
    Node<int> current_node;
    bool done = false;
    int internal_count = 0;
    queue<Node<int>> next_frontier;
    chrono::microseconds us = 500us; // delay
    do{
        bool process = true;
        do{
            
           
            {
               
                utimer ut("Time Pop", debug);
                if(steal)
                global_queue_mutex[id].lock();
                current_node = current_frontier[id].front();
                current_frontier[id].pop();
                if(current_frontier[id].empty()) process=false;
                if(steal)
                global_queue_mutex[id].unlock();
                
            }
            
            if(current_node.get_value()==value) internal_count++;
            vector<Edge> children = current_node.getChild();
            {
                utimer ut("Time Edges", debug);
                if(wait) delay(us);
                for(int i=0;i<children.size();i++){
                    int tmp_id = children[i].get_edge_id();

                    visited_mutex[tmp_id].lock();
                    bool tmp_visited = visited[tmp_id];
                    
                    if(!tmp_visited){
                        visited[tmp_id]=true;
                        next_frontier.push(graph.getNode(tmp_id));
                    }
                    visited_mutex[tmp_id].unlock();
                }
            }
          
        }while(process);
        //JOB STEALING
        if(steal){
                {
                    utimer("Time stealing", debug);
                    for(int i=0;i != id && i<num_workers;i++){
                        global_queue_mutex[i].lock();
                        
                        if((current_frontier[i].size()>100)){
                            if(debug){
                                std::cout << "Stealing\n";
                            }
                            for(int j=0;j<50;j++){ 
                                current_frontier[id].push(graph.getNode(current_frontier[i].front().get_node_id()));
                                current_frontier[i].pop();
                            }
                            global_queue_mutex[i].unlock();
                            break;
                        }
                        global_queue_mutex[i].unlock();
                    }
                }
        }
        
       {
            if(steal)
            global_queue_mutex[id].lock();
            if(current_frontier[id].empty())
            {
                if(steal)
                global_queue_mutex[id].unlock();
                {
                    utimer ut("Time Barrier", debug);
                    barrier.wait(id);
                }
                {
                    utimer ut("Time Swap&Check", debug);
                    if(steal)
                    global_queue_mutex[id].lock();
                    current_frontier[id].swap( next_frontier);
                    if(current_frontier[id].empty()){
                        
                        barrier.dec();
                        done=true;
                    } 
                    if(steal)
                    global_queue_mutex[id].unlock();
                }
               
            }else {
                 if(steal)
                global_queue_mutex[id].unlock();
            }
        }
        
       
           
    }while(!done);
    {
        utimer ut("Time counter", debug);
        global_counter += internal_count; //OPERATION DONE ATOMICALLY!
     
    }
}

class Worker : public ff_node
{
    private:
        int t;

    public:
        Worker(int tt = -1)
        {
            t = tt;
        }
        int svc_init()
        {
            if (t > -1)
                set_id(t);
            return 0;
        }
        void set_id(ssize_t id)
        {
            ff_node::set_id(id);
        }
        int run() { return ff_node::run(); }
        int wait() { return ff_node::wait(); }
        void *svc(void *)
        {
            
            BFS(
                get_my_id()
            );
            return 0;
        }
};

int main(int argc, char *argv[])
{
    if(argc < 9){
        cout << "Usage: ./ff num_nodes min_edges max_edges start_node value num_workers steal(0/1) active_wait(0/1) debug(0/1)\n";
        return 0;
    }
    num_nodes = atoi(argv[1]);
    min_edges = atoi(argv[2]);
    max_edges = atoi(argv[3]);
    start_node = atoi(argv[4]);
    value = atoi(argv[5]);
    num_workers = atoi(argv[6]);
    steal = atoi(argv[7]);
    wait=atoi(argv[8]);
    debug=atoi(argv[9]);


    std::vector<std::mutex> list(((size_t) num_workers));
    barrier.setCounter(num_workers);
    global_queue_mutex.swap(list);
    std::vector<std::mutex> list2(((size_t) num_nodes));
    visited_mutex.swap(list2);
    std::vector<bool> list3(((size_t) num_nodes));
    visited.swap(list3);


    if (start_node>num_nodes)
    {
        std::cout << "Starting node " << start_node << " not exists\n";
        return 0;
    }
    string filename = "graph_generation/data_" + to_string(num_nodes) + "_" + to_string(min_edges) + "_" + to_string(max_edges) + ".txt";
    graph = process(filename, num_nodes, start_node);

    Node<int> source = graph.getNode(start_node);
   
    current_frontier.resize(num_workers);

    visited[source.get_node_id()] = true;
    if (source.get_value() == value)
        global_counter++;

    vector<Edge> init_edges = source.getChild();
    {
        utimer ut("Time initialization", true);
        for (int i = 0; i < init_edges.size(); i++)
        {
            int edge_id = init_edges[i].get_edge_id();
            if (!visited[edge_id])
            {
                if(num_workers>1){
                    current_frontier[i % num_workers].push(graph.getNode(edge_id));
                }
                else{
                      current_frontier[0].push(graph.getNode(edge_id));
                }
                visited[edge_id]=true;
            }
        }
    }
    {
        utimer ut("Time Fastflow", true);
        ff_farm farm;
        vector<ff_node *> w;
        for (int i = 0; i < num_workers; i++)
        {
            w.push_back(new Worker(i));
        }
        farm.add_workers(w);
        if (farm.run_then_freeze() < 0)
        {
            cout << "Error";
            return 0;
        }
        else
        {
            farm.wait_freezing();
        }

    }

   cout << "Found values equal to "<< value << ": " << global_counter << endl;
    return 0;
}