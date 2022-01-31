# Searching on graphs
BFS parallel implementation.


Breath-First Search (from now on I will refer to it as BFS) is a well-known
algorithm used to process graphs. The computation maintains two frontiers: the
first one is used to process the current level of nodes, where the latter is used to
keep track of the children’s nodes of the previous frontier. The algorithm repeats
until the next frontier is empty. Since each nodes is independent from the others
we focus on a particular kind of Data-Parallel Problem: the embarrassingly-
parallel one.

### Prerequisites
  * Download Fastflow into the main folder
  See the BUILD.ME file for instructions about building unit tests and examples.
  ```sh
  https://github.com/fastflow/fastflow
  ```
* compiling generation graph
  ```sh
      g++ -std=c++17 gen graph.cpp -o gen_graph
  ```
  
  * compiling sequential code
  ```sh
      g++ -std=c++17 -O3 seq.cpp -o seq
  ```
  
  * compiling parallel code (StandardLibrary)
  ```sh
      g++ -std=c++17 -O3 -pthread par.cpp -o par
  ```
  
  * compiling parallel code (Fastflow)
  ```sh
      g++ -std=c++17 -O3 -pthread -I fastflow ff.cpp -o ff
  ```
### Usage

* Generate graphs
   ```sh
   ./gen_graph total nodes min edges max edges max value seed
   ```
* Sequential Execution
   ```sh
   ./seq num_nodes min_edges max_edges start_node value active_wait(0/1) debug(0/1)
   ```
   
* Parallel Execution (StandardLibrary)
   ```js
   ./par num_nodes min_edges max_edges start_node value num_workers steal(0/1) active_wait(0/1) debug(0/1)
   ```
* Parallel Execution (Fastflow)
   ```js
   ./ff num_nodes min_edges max_edges start_node value num_workers steal(0/1) active_wait(0/1) debug(0/1)
   ```

