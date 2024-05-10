#include "kd_range_search.h"                //range search decls

//----------------------------------------------------------------------
//		To keep argument lists short, a number of global variables
//		are maintained which are common to all the recursive calls.
//		These are given below.
//----------------------------------------------------------------------

ANNpointArray	     ANNkdRSPts;			// the points
std::vector<Point>   PolyPtsReturned;         // set of points in polygon (returned)

std::vector<Point>& ANNkd_tree::annRangeSearch(Polygon& polygon) //input polygon
{
    
}

void ANNkd_split::ann_range_search(Polygon& polygon)
{

}

void ANNkd_leaf::ann_range_search(Polygon& polygon)
{

}



