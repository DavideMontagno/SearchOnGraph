#include <cstdio>
#include <thread>
#include <atomic>
#include <iostream>
#include "./utility/barrier.cpp"
#include "./utility/node.cpp"
#include "./utility/graph.cpp"
#include "./utility/utimer.cpp"
#include "./utility/processing_graph.cpp"
int stop=0;

using namespace std;

std::vector<std::mutex> global_queue_mutex;
std::vector<std::mutex>  visited_mutex;


//THREAD FUNCTION
void BFS(Graph<int> &graph,vector<bool> &visited, MyBarrier *barrier, int value,atomic<int> &global_counter,vector<queue<Node<int>>> &current_frontier,
          int num_workers, int id, int steal, int wait, int debug){
    bool stop=false;
    Node<int> current_node;
    bool done = false;
    queue<Node<int>> next_frontier;
   
    int internal_count = 0;

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
                    barrier->wait(id);
                }
                {
                    utimer ut("Time Swap&Check", debug);
                    if(steal)
                    global_queue_mutex[id].lock();
                    current_frontier[id].swap( next_frontier);
                    if(current_frontier[id].empty()){
                        barrier->dec();
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


int main(int argc, char**argv){
    
    if(argc < 9){
        cout << "Usage: ./par num_nodes min_edges max_edges start_node value num_workers steal(0/1) active_wait(0/1) debug(0/1)\n";
        return 0;
    }
    int num_nodes = atoi(argv[1]);
    int min_edges = atoi(argv[2]);
    int max_edges = atoi(argv[3]);
    int start_node = atoi(argv[4]);
    int value = atoi(argv[5]);
    int num_workers = atoi(argv[6]);
    int steal = atoi(argv[7]);
    int active_wait=atoi(argv[8]);
    int debug=atoi(argv[9]);
    std::vector<std::mutex> list(((size_t) num_workers));
    global_queue_mutex.swap(list);
    std::vector<std::mutex> list2(((size_t) num_nodes));
    visited_mutex.swap(list2);
    vector<thread> threads;
    std::atomic<int> global_counter = 0;
    MyBarrier barrier(num_workers);
    //LOCAL VARIABLE NECESSARY TO SYNCHRONIZE THE THREAD.
    
    std::thread t[num_workers]; 
    int i=0;
    string filename = "graph_generation/data_" + to_string(num_nodes) + "_" + to_string(min_edges) + "_" + to_string(max_edges) + ".txt";

    //TAKING GRAPH FROM FILE
    Graph<int> graph;
    if (start_node>num_nodes)
    {
        std::cout << "Starting node " << start_node << " not exists\n";
        return 0;
    }
    graph = process(filename, num_nodes, start_node);

    vector<bool> visited(num_nodes);
    Node<int> source = graph.getNode(start_node);
    vector<queue<Node<int>>> current_frontier(num_workers);
    vector<queue<Node<int>>> next_frontier(num_workers);

    visited[source.get_node_id()] = true;
    if (source.get_value() == value)
    {
        global_counter++;
    }

    {
        utimer ut("Time initilization", true);
        vector<Edge> init_edges = source.getChild();
        for(int i = 0; i < init_edges.size(); i++) {
            int edge_id = init_edges[i].get_edge_id();
            if (!visited[edge_id])
            {
                //ROUND ROBIN
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
    //START AND JOINING ALL THREADS
    {
         utimer ut("Time Parallel", true);
         for(size_t i = 0; i < num_workers; i++) t[i] = std::thread(BFS, 
                std::ref(graph),
                std::ref(visited),
                &barrier,
                value,
                std::ref(global_counter),
                std::ref(current_frontier),
                num_workers,
                i,
                steal,
                active_wait,
                debug);
    for(size_t i = 0; i < num_workers; i++) t[i].join();
    }
   
     cout << "Found values equal to "<< value << ": " << global_counter << endl;
}

   