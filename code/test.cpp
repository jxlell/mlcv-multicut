#include <iostream>
#include <vector>
#include <string>
#include <boost/graph/adjacency_list.hpp>
#include <opencv2/opencv.hpp> 

using namespace std;

// Define a struct to represent vertex properties
struct VertexProperties {
    int id;
    std::string name;
};


bool isAdj(std::vector<std::vector<std::string>> a, std::vector<std::vector<std::string>> b){
    return true;
}

int main()
{
    const int rows = 5;
    const int columns = 5;
    std::vector<std::vector<std::string>> exampleImg(rows, std::vector<std::string>(columns)); 



    /*
    // Define a 2D grid graph with 3 rows and 4 columns
    typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS> Graph;
    const int num_rows = 3;
    const int num_cols = 4;

    

    // Create a grid graph object
    Graph grid(num_rows * num_cols);

    typedef boost::property_map<Graph, int VertexProperties::*>::type VertexIdMap;
    VertexIdMap vertexIdMap = boost::get(&VertexProperties::id, grid);

    // Add edges for horizontal connections
    for (int row = 0; row < num_rows; ++row) {
        for (int col = 0; col < num_cols - 1; ++col) {
            boost::add_edge(row * num_cols + col, row * num_cols + col + 1, grid);
        }
    }

    // Add edges for vertical connections
    for (int row = 0; row < num_rows - 1; ++row) {
        for (int col = 0; col < num_cols; ++col) {
            boost::add_edge(row * num_cols + col, (row + 1) * num_cols + col, grid);
        }
    }

    // Iterate through vertices and edges
    Graph::vertex_iterator vi, vend;
    for (boost::tie(vi, vend) = boost::vertices(grid); vi != vend; ++vi) {
        std::cout << "Vertex: " << *vi << std::endl;
    }

    Graph::edge_iterator ei, eend;
    for (boost::tie(ei, eend) = boost::edges(grid); ei != eend; ++ei) {
        std::cout << "Edge: " << *ei << std::endl;
    }
    */
   return 0;
    
}