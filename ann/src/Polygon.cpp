// Polygon.cpp
#include <ANN/ANN.h>


Polygon::Polygon(std::vector<Point>& polygon_vertices, int m) : m(polygon_vertices.size()) {
    //deep copy of points
    vertices.clear();
    for (int i = 0;i < m;++i) {
        Point point(polygon_vertices[i]);
        vertices.push_back(point);
    }
    
    //set low and high bounds in each dimension
    int d = 2;
    low = annAllocPt(d);
    high = annAllocPt(d);
    //initialize low, high
    low[0] = vertices[0].annPoint[0];
    low[1]= vertices[0].annPoint[1];
    high[0]= vertices[0].annPoint[0];
    high[1] = vertices[0].annPoint[1];
    //update low and high bounds in each dimension
    for (int i = 1;i < vertices.size();++i) {
        if (vertices[i].annPoint[0] < low[0]) {
            low[0] = vertices[i].annPoint[0];
        }
        else if (vertices[i].annPoint[0] > high[0]) {
            high[0]= vertices[i].annPoint[0];
        }
        if (vertices[i].annPoint[1] < low[1]) {
            low[1] = vertices[i].annPoint[1];
        }
        else if (vertices[i].annPoint[1] > high[1]) {
            high[1] = vertices[i].annPoint[1];
        }
    }
}

Polygon::~Polygon() {
    // Assuming the vertices are dynamically allocated elsewhere
}