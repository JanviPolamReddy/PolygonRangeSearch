/**
 * @file range_search.cpp
 * @brief Range search implementation using kd-tree from the ANN library.
 */
#include <iostream>
#include <fstream>
#include <ANN/ANN.h>
#include <chrono>
#include <iostream>
#include <filesystem>
#include "json.hpp"
using json = nlohmann::json;
using namespace std;
namespace fs = std::filesystem;

/**
 * @brief Writes the inside counts to a JSON file.
 * @param insideCounts The vector of inside counts.
 * @param outputFilePath The path to the output JSON file.
 */
void writeInsideCountsToJson(const std::vector<unsigned int>& insideCounts, const std::string& outputFilePath) {
    // Convert the vector to a JSON array
    nlohmann::json jsonArray = insideCounts;

    // Open the output file
    std::ofstream outputFile(outputFilePath);
    if (outputFile.is_open()) {
        // Write the JSON array to the file
        outputFile << jsonArray.dump(4);
        outputFile.close();
    } else {
        std::cout << "Unable to open file for writing: " << outputFilePath << std::endl;
    }
}

/**
 * @brief Writes the inside points indices to a JSON file.
 * @param insidePointsIndices The vector of inside points indices.
 * @param outputFilePath The path to the output JSON file.
 */
void writeInsidePointsIndicesToJson(const std::vector<std::vector<unsigned int>>& insidePointsIndices, const std::string& outputFilePath) {
    // Convert the vector of vectors to a JSON array
    nlohmann::json jsonArray = insidePointsIndices;

    // Open the output file
    std::ofstream outputFile(outputFilePath);
    if (outputFile.is_open()) {
        // Write the JSON array to the file
        outputFile << jsonArray.dump(4);
        outputFile.close();
    } else {
        std::cout << "Unable to open file for writing: " << outputFilePath << std::endl;
    }
}

/**
 * @brief Compares two JSON files for equality.
 * @param filePath1 The path to the first JSON file.
 * @param filePath2 The path to the second JSON file.
 * @return True if the JSON files are the same, false otherwise.
 */
bool areJsonFilesSame(const std::string& filePath1, const std::string& filePath2, bool countCheck) {
    std::ifstream file1(filePath1);
    std::ifstream file2(filePath2);

    if (!file1.is_open() || !file2.is_open()) {
        return false; // File could not be opened
    }

    nlohmann::json json1, json2;
    try {
        file1 >> json1;
        file2 >> json2;
    } catch (nlohmann::json::parse_error& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        return false; // JSON parsing failed
    }

    if (countCheck) {
        // Check if the JSON data are arrays
        if (json1.is_array() && json2.is_array()) {
            // Sort both arrays
            std::sort(json1.begin(), json1.end());
            std::sort(json2.begin(), json2.end());
        }
    } else {
        if (json1.is_array() && json2.is_array()) {
        // Sort elements within each sub-array
        for (auto& arr : json1) {
            std::sort(arr.begin(), arr.end());
        }
        for (auto& arr : json2) {
            std::sort(arr.begin(), arr.end());
        }
    }
    }
    
    return json1 == json2; // Compare JSON objects
}

/**
 * @brief Loads points and polygons from JSON files.
 * @param pointFilePath The path to the JSON file containing points.
 * @param queryFilePath The path to the JSON file containing polygons.
 * @param points The vector to store the loaded points.
 * @param polygons The vector to store the loaded polygons.
 */
void loadFromFile(const std::string& pointFilePath, const std::string& queryFilePath, std::vector<Point>& points, std::vector<std::vector<Point>>& polygons) {
    std::ifstream f(pointFilePath);
    json data = json::parse(f);

    //read points
    int i = 0;
    for (auto p : data) {
        Point point(i++, p[0], p[1]);
        points.push_back(point);
    }
    f.close();

    f.open(queryFilePath);
    data = json::parse(f);

    //read polygon vertices and create polygons
    for (auto poly : data) {
        std::vector<Point> polygon_vertices;
        int j = 0;
        for (auto p : poly) {
            polygon_vertices.emplace_back(j++, p[0], p[1]);
        }
        polygons.push_back(polygon_vertices);
    }

    f.close();

    return;
}

/**
 * @brief Checks if a point is contained within a polygon.
 * @param vertices The vertices of the polygon.
 * @param p The point to check.
 * @return True if the point is inside the polygon, false otherwise.
 */
bool containsPoint(const std::vector<Point>& vertices, const Point& p) {
    int count = 0;
    int n = vertices.size();

    for (int i = 0; i < n; i++) {
        int j = (i + 1) % n;

        // Check if the point is to the left of the edge when moving from vertex i to vertex j
        if ((vertices[i].annPoint[1] > p.annPoint[1]) != (vertices[j].annPoint[1] > p.annPoint[1])) {
            double xIntersect = (vertices[j].annPoint[0] - vertices[i].annPoint[0]) * (p.annPoint[1] - vertices[i].annPoint[1]) / (vertices[j].annPoint[1] - vertices[i].annPoint[1]) + vertices[i].annPoint[0];
            if (p.annPoint[0] < xIntersect) {
                count++;
            }
        }
    }

    return (count % 2 == 1);  // True if count is odd
}

/**
 * @brief Compute the optimal search radius for a given polygon and its midpoint.
 *
 * This function calculates the maximum distance from the midpoint of the polygon
 * to any of its vertices. The maximum distance is used as the optimal search radius
 * for range search queries.
 *
 * @param polygon The polygon for which to compute the optimal search radius.
 * @param mid The midpoint of the polygon.
 * @return The optimal search radius.
 */
double computeOptimalSearchRadius(const Polygon& polygon, ANNpoint mid) {
    double maxDistance = 0.0;
    for (int dim = 0; dim < 2; ++dim) {
        double dist1 = fabs(polygon.high[dim] - mid[dim]);
        double dist2 = fabs(polygon.low[dim] - mid[dim]);
        maxDistance = std::max(maxDistance, std::max(dist1, dist2));
    }
    return maxDistance;
}

/**
 * @brief Performs range search tests using input files and writes the results to output files.
 * @param pointFilePath The path to the JSON file containing points.
 * @param queryFilePath The path to the JSON file containing polygons.
 * @param testFolderPath The path to the test folder.
 */
void testRangeSearchFromFile(const std::string& pointFilePath, const std::string& queryFilePath, const std::string& testFolderPath) {
    std::vector<Point> points;
    std::vector<std::vector<Point>> polygons;

    // Start timer for preprocessing
    auto startPreprocessing = std::chrono::high_resolution_clock::now();

    //loads points into a vector of Point and polygon into a Polygon object
    loadFromFile(pointFilePath, queryFilePath, points, polygons);
    ANNpointArray dataPts = annAllocPts(points.size(), 2);

    // Convert points to ANNpointArray format
    for (size_t i = 0; i < points.size(); ++i) {
        dataPts[i][0] = points[i].annPoint[0];  //x-coordinate
        dataPts[i][1] = points[i].annPoint[1];  //y-coordinate
    }

    // Build the kd-tree using the converted data points
    ANNkd_tree* kdTree = new ANNkd_tree(dataPts, points.size(), 2);
    
    auto endPreprocessing = std::chrono::high_resolution_clock::now();
    auto preprocessingTime = std::chrono::duration_cast<std::chrono::milliseconds>(endPreprocessing - startPreprocessing).count();

    std::vector<unsigned int> insideCounts;
    std::vector<std::vector<unsigned int>> insidePointsIndices;

    // Placeholder: Measure query time
    auto startQuery = std::chrono::high_resolution_clock::now();

    // Perform all the polygon queries on input points
    for (std::vector<Point> polygon_vertices : polygons) {
        Polygon polygon(polygon_vertices, (int)polygon_vertices.size());

        // Define the bounding box for range search
        ANNpoint low = annAllocPt(2), high = annAllocPt(2), mid = annAllocPt(2);
        low[0] = polygon.low[0]; low[1] = polygon.low[1];
        high[0] = polygon.high[0]; high[1] = polygon.high[1];
        mid[0] =  (polygon.low[0] + polygon.high[0])/2; mid[1] =  (polygon.low[1] + polygon.high[1])/2;
        ANNidxArray nnIdx = new ANNidx[points.size()]; // Array for storing indices of points in range
        ANNdistArray dists = new ANNdist[points.size()]; // Array for storing distances to points in range

        // Define a search radius to cover the entire bounding box
        double largeRadius = computeOptimalSearchRadius(polygon, mid);
        
        // Perform the search within the bounding box using a fixed radius
        int pointsFound = kdTree->annkFRSearch(mid, largeRadius, points.size(), nnIdx, dists);

        unsigned int insideCount = 0;
        std::vector<unsigned int> indices;
        // Check which of these points are actually inside the polygon
        for (int i = 0; i < pointsFound; ++i) {
            if (containsPoint(polygon_vertices, points[nnIdx[i]])) {
                indices.push_back(nnIdx[i]);
            }
        }

        insideCounts.push_back(indices.size());
        insidePointsIndices.push_back(indices);
    }
    

    auto endQuery = std::chrono::high_resolution_clock::now();
    auto queryTime = std::chrono::duration_cast<std::chrono::milliseconds>(endQuery - startQuery).count();

   // Write output to files
   writeInsideCountsToJson(insideCounts, testFolderPath + "/generated_output_count.json");
   writeInsidePointsIndicesToJson(insidePointsIndices, testFolderPath + "/generated_output_report.json");

    // Output results - timings and accuracy
    if (areJsonFilesSame(testFolderPath + "/generated_output_count.json", testFolderPath + "/output_count.json", true)) {
        std::cout << "  Count Accuracy: 100%" << std::endl;
    } else {
        std::cout << "  Count Accuracy: Failed" << std::endl;
    }
    if (areJsonFilesSame(testFolderPath + "/generated_output_report.json", testFolderPath + "/output_report.json", false)) {
        std::cout << "  Range Accuracy: 100%" << std::endl;
    } else {
        std::cout << "  Range Accuracy: Failed" << std::endl;
    }
    std::cout << "  Preprocessing Time: " << preprocessingTime << " milliseconds" << std::endl;
    std::cout << "  Query Time: " << queryTime << " milliseconds" << std::endl;
    std::cout << "  Total Time: " << (preprocessingTime + queryTime) << " milliseconds" << std::endl;

}

/**
 * @brief Main function.
 * @return 0 on successful execution.
 */
int main() {
    std::string basePath = "../";

    // Iterate over entries in the base path and get the folders starting with test
    for (const auto& entry : fs::directory_iterator(basePath)) {
        if (entry.is_directory() && entry.path().filename().string().find("test") == 0) {
            std::string folderPath = entry.path().string();
            std::string pointFilePath = folderPath + "/input_points.json";
            std::string queryFilePath = folderPath + "/input_query.json";

            std::cout << "Processing: " << folderPath << std::endl;
            testRangeSearchFromFile(pointFilePath, queryFilePath, folderPath);
        }
    }
    return 0;
}
