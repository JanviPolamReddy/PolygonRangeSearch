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
1.Download the project zip file and extract it to your desired location.
2.Open a terminal or command prompt and navigate to the project folder:
cd /path/to/your/project/folder
3.Once you are inside the project folder, compile the project by running the following command:
make
4.This will compile the source files and generate the executable.

Execution:
After successfully compiling the project, you can run the range search executable using the following commands:
chmod +x rangesearch
./rangesearch
This command will execute the code for all the test cases and display the output in the terminal.

TestCases:
The project includes test cases to validate the implementation of the range search algorithm. When you run the ./rangesearch command, it automatically executes all the test cases and prints the corresponding output.
