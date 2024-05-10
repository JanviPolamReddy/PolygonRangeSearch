#Polygon Range Search using kd-trees

This project demonstrates the implementation of polygon range search using kd-trees. The program reads input points and polygons from JSON files, performs range searches to find the points inside each polygon, and outputs the results to JSON files.

Functions:
writeInsideCountsToJson: Writes the counts of points inside each polygon to a JSON file.
writeInsidePointsIndicesToJson: Writes the indices of points inside each polygon to a JSON file.
areJsonFilesSame: Compares two JSON files to check if they are identical.
loadFromFile: Loads input points and polygons from JSON files into vectors.
containsPoint: Determines if a given point lies inside a polygon using the ray casting algorithm.
testRangeSearchFromFile: Performs polygon range search on input points and polygons, and outputs the results.
main: Iterates over test folders, calls testRangeSearchFromFile for each test case, and reports accuracy and performance.

Compilation:



Execution: