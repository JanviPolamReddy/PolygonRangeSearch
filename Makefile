CXX=g++
CXXFLAGS=-std=c++17 -I./ann/include -I./ann/src 
# Included user-defined headers (Point.cpp and Polygon.cpp) related to the ANN library.
LDFLAGS=-L./ann/lib -lANN -lm
SRC=main.cpp ann/src/Point.cpp ann/src/Polygon.cpp 
# Included User-defined source files in ANN library
OBJ=$(SRC:.cpp=.o)
EXEC=rangesearch

all: ann $(EXEC)

$(EXEC): $(OBJ)
	$(CXX) -o $@ $(OBJ) $(CXXFLAGS) $(LDFLAGS)

%.o: %.cpp
	$(CXX) -c $< $(CXXFLAGS)

ann:
	if [ ! -d "./ann" ]; then \
		curl -o ann_1.1.2.tar.gz https://www.cs.umd.edu/~mount/ANN/Files/1.1.2/ann_1.1.2.tar.gz; \
		tar zxf ann_1.1.2.tar.gz; \
		mv ann_1.1.2 ann; \
		(cd ann && make linux-g++ && cd ..); \
		rm ann_1.1.2.tar.gz; \
	fi

clean:
	rm -f $(OBJ) $(EXEC)
	rm -rf ann*

.PHONY: all clean ann
