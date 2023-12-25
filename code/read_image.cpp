#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;

/*
struct RGB {
    int red;
    int green;
    int blue;
};
*/

struct RGB {
    std::bitset<8> red;
    std::bitset<8> green;
    std::bitset<8> blue;
};


class Graph{
    public:
        Graph(int vertices);
        void addEdge(int v, int w);
        void addVertex();
        void printGraph();
        int getVertices() const {
            return vertices;
        }
        vector<vector<int>> getadjList() const {
            return adjList;
        }
        // Setter method to set the RGB value for a vertex
        void setVertexColor(int v, int red, int green, int blue);

        // Getter method to access the RGB value for a vertex
        RGB getVertexColor(int v) const;

        // Setter method to set the value for a vertex
        void setVertexValue(int v, int value);

        // Getter method to access the value for a vertex
        int getVertexValue(int v) const;

        bool compareRGB(const RGB& color1, const RGB& color2);

        int countEdges() const;

        // Function to set the bit for a specific edge
        void setEdgeBit(int v, int w, bool value);

        // Function to get the bit for a specific edge
        bool getEdgeBit(int v, int w) const;

    private:
        int vertices;
        vector<vector<int>> adjList;
        vector<RGB> vertexColors; 
        vector<int> vertexValues;
        // Vector of bitsets to store the multicut
        std::vector<std::bitset<1>> edgeBits;


};

Graph::Graph(int vertices) {
    this->vertices = vertices;
    adjList.resize(vertices);
    vertexValues.resize(vertices, 0); 
    vertexColors.resize(vertices);    
    edgeBits.resize(100, std::bitset<1>(0));
}


// Function to add an edge to the graph
void Graph::addEdge(int v, int w) {
    if (find(adjList[v].begin(), adjList[v].end(), w) == adjList[v].end()) {
        adjList[v].push_back(w); // Add w to v's list
        adjList[w].push_back(v); // Add v to w's list (for an undirected graph)
    }
}


// Function to add a vertex to the graph
void Graph::addVertex() {
    vertices++;
    adjList.push_back(vector<int>()); // Add an empty vector for the new vertex
}

// Function to print the graph
void Graph::printGraph() {
    for (int v = 0; v < vertices; ++v) {
        cout << "vertex " << v << ": " << endl ;
        cout << "Adjacency list: ";
        for (const auto &neighbor : adjList[v]) {
            cout << neighbor << "(" << getEdgeBit(v,neighbor) << ")" << " ";
        }
        RGB color = getVertexColor(v);
        cout << endl << "Color (RGB): " << color.red.to_ulong() << ", " << color.green.to_ulong() << ", " << color.blue.to_ulong()
             << endl;
        
    }
}


// Setter method to set the RGB value for a vertex
void Graph::setVertexColor(int v, int red, int green, int blue) {
    if (v >= 0 && v < vertices) {
        vertexColors[v].red = std::bitset<8>(red);
        vertexColors[v].green = std::bitset<8>(green);
        vertexColors[v].blue = std::bitset<8>(blue);
    } else {
        cout << "Invalid vertex index." << endl;
    }
}

// Getter method to access the RGB value for a vertex
RGB Graph::getVertexColor(int v) const {
    if (v >= 0 && v < vertices) {
        return vertexColors[v];
    } else {
        cout << "Invalid vertex index. Returning (0, 0, 0)." << endl;
        return {std::bitset<8>(0), std::bitset<8>(0), std::bitset<8>(0)};
    }
}

// Setter method to set the value for a vertex
void Graph::setVertexValue(int v, int value) {
    if (v >= 0 && v < vertices) {
        vertexValues[v] = value;
    } else {
        cout << "Invalid vertex index." << endl;
    }
}

// Getter method to access the value for a vertex
int Graph::getVertexValue(int v) const {
    if (v >= 0 && v < vertices) {
        return vertexValues[v];
    } else {
        cout << "Invalid vertex index. Returning 0." << endl;
        return 0;
    }
}

/**
 * @brief Compare two RGB colors for equality.
 *
 * This function compares two RGB colors by comparing their individual color components.
 *
 * @param color1 The first RGB color to compare.
 * @param color2 The second RGB color to compare.
 * @return true if the colors are equal, false otherwise.
 */
bool Graph::compareRGB(const RGB& color1, const RGB& color2) {
    return (color1.red == color2.red) && (color1.green == color2.green) && (color1.blue == color2.blue);
}

int Graph::countEdges() const {
    int edgeCount = 0;

    // Iterate through each vertex
    for (int v = 0; v < vertices; ++v) {
        // Increment the edge count by the number of adjacent vertices
        edgeCount += adjList[v].size();
    }

    // For an undirected graph, each edge is counted twice, so divide by 2
    return edgeCount / 2;
}

// Implement the setEdgeBit function
void Graph::setEdgeBit(int v, int w, bool value) {
    int edgeIndex = find(adjList[v].begin(), adjList[v].end(), w) - adjList[v].begin();
    
    if (edgeIndex < adjList[v].size()) {
        // If the edge exists, set the bit for the corresponding edge
        edgeBits[v][edgeIndex] = value;
    } else {
        // Handle the case where the edge does not exist
        std::cerr << "Edge does not exist." << std::endl;
    }
}

// Implement the getEdgeBit function
bool Graph::getEdgeBit(int v, int w) const {
    int edgeIndex = find(adjList[v].begin(), adjList[v].end(), w) - adjList[v].begin();

    if (edgeIndex < adjList[v].size()) {
        // If the edge exists, return the bit for the corresponding edge
        return edgeBits[v][edgeIndex];
    } else {
        // Handle the case where the edge does not exist
        std::cerr << "Edge does not exist." << std::endl;
        return false; // Or handle this case differently based on your requirements
    }
}

int main() {
    
    /*

    */
    int rows, cols = 5;

    //cout << rows*cols << endl;

    // Create a graph with 5 vertices
    Graph graph(rows*cols);


    // set "colors" for all vertices (example image from task pdf)
    /*
    graph.setVertexValue(0,1);
    graph.setVertexValue(1,1);
    graph.setVertexValue(2,1);
    graph.setVertexValue(3,1);
    graph.setVertexValue(4,1);
    graph.setVertexValue(5,1);
    graph.setVertexValue(6,1);
    graph.setVertexValue(7,2);
    graph.setVertexValue(8,1);
    graph.setVertexValue(9,1);
    graph.setVertexValue(10,1);
    graph.setVertexValue(11,2);
    graph.setVertexValue(12,2);
    graph.setVertexValue(13,2);
    graph.setVertexValue(14,1);
    graph.setVertexValue(15,1);
    graph.setVertexValue(16,1);
    graph.setVertexValue(17,3);
    graph.setVertexValue(18,1);
    graph.setVertexValue(19,1);
    graph.setVertexValue(20,4);
    graph.setVertexValue(21,4);
    graph.setVertexValue(22,4);
    graph.setVertexValue(23,4);
    graph.setVertexValue(24,4);
    */
    

    /*
    for (int v = 0; v < graph.getVertices(); ++v) {
        cout << "Adjacency list of vertex " << v << ": ";
        vector<vector<int>> adjList = graph.getadjList();
        for (const auto &neighbor : adjList[v]) {
            cout << neighbor << " ";
        }
        cout << endl;
    }
    */


    const vector<int> neighborsOffsets = {-cols, cols, -1, 1};

    /*
    // Check neighbors for every vertex and set multicut edges
    for (int v = 0; v < graph.getVertices(); ++v) {
        for (int offset : neighborsOffsets) {
            int neighbor = v + offset;
            if (neighbor >= 0 && neighbor < graph.getVertices() &&
                graph.getVertexValue(neighbor) != graph.getVertexValue(v)) {
                graph.addEdge(neighbor, v);
            }
        }
    }
    */
    
    
 

    // Print the graph
    //graph.printGraph();


    //#########################################
    // same example but with reading an image + RGB values

    

    //cv::Mat img = cv::imread("/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV Project/code/tree5x5.png", 
    //cv::IMREAD_UNCHANGED /*cv::IMREAD_COLOR*/);

    cv::Mat img = cv::imread("/Users/jalell/Library/CloudStorage/OneDrive-Persönlich/SURFACE/TuDD/MASTER/MLCV Project/code/tree5x5.jpg", 
    /*cv::IMREAD_UNCHANGED*/ cv::IMREAD_COLOR);

    if (img.empty()) {
        std::cerr << "Error: Could not read the image." << std::endl;
        return -1;
    }

    rows = img.rows;
    cols = img.cols;

    Graph img_graph(rows*cols);


    // Iterate through every pixel from top-left to bottom-right
    int pixel_index = 0;
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            // Access pixel value at position (x, y)
            cv::Vec3b pixel = img.at<cv::Vec3b>(y, x);

            // Access individual color channels
            uchar blue = pixel[0];
            uchar green = pixel[1];
            uchar red = pixel[2];

            //cout << "Pixel at (" << x << ", " << y << "): ";
            //cout << "B: " << static_cast<int>(blue) << ", ";
            //cout << "G: " << static_cast<int>(green) << ", ";
            //cout << "R: " << static_cast<int>(red) << endl;

            //set rgb values for vertex
            //img_graph.setVertexColor(pixel_index, static_cast<int>(red), static_cast<int>(blue), static_cast<int>(green));
            img_graph.setVertexColor(y*cols+x, static_cast<int>(red), static_cast<int>(blue), static_cast<int>(green));
            pixel_index++;
        }
    }

    
    // Check neighbors for every vertex and set multicut bits
    for (int v = 0; v < img_graph.getVertices(); ++v) {
        for (int offset : neighborsOffsets) {
            int neighbor = v + offset;
            if (neighbor >= 0 && neighbor < img_graph.getVertices() && ((neighbor / cols == v / cols) || (abs(neighbor - v) > 1))) {
                img_graph.addEdge(neighbor, v);
                if(!img_graph.compareRGB(img_graph.getVertexColor(v), img_graph.getVertexColor(neighbor))){
                // ###### add multicut bit 
                img_graph.setEdgeBit(v,neighbor,1);
                }
            }
        }
    }
    

    img_graph.printGraph();

    cout << "number of edges: " << img_graph.countEdges() << endl;

    //cv::imshow("Image", img);
    //cv::waitKey(3000);

    return 0;
}