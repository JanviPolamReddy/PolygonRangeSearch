// Point.cpp
#include <ANN/ANN.h>


Point::Point(int id, ANNcoord x, ANNcoord y) : id(id) {
    annPoint = annAllocPt(2); // Assuming 2D points
    annPoint[0] = x;
    annPoint[1] = y;
}

//copy constructor: deep copy
Point::Point(const Point& point ) {
    id = point.id;
    annPoint = annAllocPt(2); // Assuming 2D points
    annPoint[0] = point.annPoint[0];
    annPoint[1] = point.annPoint[1];
}

Point::~Point() {
    annDeallocPt(annPoint);
}

ANNpointArray Point::ConvertToANNpointArray(const std::vector<Point>& points, int n) {
    ANNpointArray pa = annAllocPts(n, 2);
    for (int i = 0; i < n; i++) {
        pa[i][0] = points[i].annPoint[0];
        pa[i][1] = points[i].annPoint[1];
    }
    return pa;
}