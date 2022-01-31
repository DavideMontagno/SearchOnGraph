#ifndef EDGE_CPP
#define EDGE_CPP
class Edge
{
    private:
        int id;
    public:
        Edge();
        Edge(const int& id);
        int get_edge_id();
};

Edge::Edge() {
    this->id = 0;
}

Edge::Edge(const int& id) {
    this->id = id;
}

int Edge::get_edge_id() {
    return this->id;
}

#endif