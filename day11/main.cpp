#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <vector>
#include <regex>
#include <unordered_map>
#include <set>
#include <queue>

using namespace std;

static const char* INPUT_FILE = "input.txt";
static const char* DEMO_FILE = "demo.txt";
static const bool USE_REAL_DATA = true;

struct Position {
    uint64_t x;
    uint64_t y;

    bool operator==(const Position& other) {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Position& other) {
        return x != other.x || y != other.y;
    }

    bool operator<(const Position& other) const {
        return y < other.y || (y == other.y && x < other.x);
    }
};

void printMap(const char* map, uint64_t w, uint64_t h) {
    for(uint64_t y = 0; y < h; y++) {
        for(uint64_t x = 0; x < w; x++) {
            cout << map[x + y*w];
        }
        cout << "\n";
    }
}

uint64_t getShortestPathManhattan(const pair<Position, Position>& pairGalaxies) {
    int64_t x = std::abs((int64_t)pairGalaxies.first.x - (int64_t)pairGalaxies.second.x);
    int64_t y = std::abs((int64_t)pairGalaxies.first.y - (int64_t)pairGalaxies.second.y);
    return x+y;
}

void getExpansion(const char* map, uint64_t width, uint64_t height, vector<uint64_t>& rowsExpanded, vector<uint64_t>& columnsExpanded) {
    // Columns
    for(uint64_t x = 0; x < width; x++) {
        uint64_t emptySpaces = 0;
        for(uint64_t y = 0; y < height; y++) {
            if(map[x + y*width] == '.')emptySpaces++;
        }
        if(emptySpaces == height) {
            columnsExpanded.push_back(x);
        }
    }
    // Rows
    for(uint64_t y = 0; y < height; y++) {
        uint64_t emptySpaces = 0;
        for(uint64_t x = 0; x < width; x++) {
            if(map[x + y*width] == '.')emptySpaces++;
        }
        if(emptySpaces == width) {
            rowsExpanded.push_back(y);
        }
    }
}

uint64_t getSumDistance(vector<Position>& vecGalaxies, const vector<uint64_t> rowsExpanded, const vector<uint64_t> columnsExpanded, uint64_t factor) {
    for(Position& galaxy: vecGalaxies) {
        uint64_t rowsExpandedBehindGalaxy = 0;
        uint64_t columnsExpandedBehindGalaxy = 0;
        for(uint64_t rowExpanded: rowsExpanded) {
            if(galaxy.y > rowExpanded) {
                rowsExpandedBehindGalaxy++;
            }
        }
        for(uint64_t columnExpanded: columnsExpanded) {
            if(galaxy.x > columnExpanded) {
                columnsExpandedBehindGalaxy++;
            }
        }

        galaxy.x += columnsExpandedBehindGalaxy * factor - columnsExpandedBehindGalaxy;
        galaxy.y += rowsExpandedBehindGalaxy * factor - rowsExpandedBehindGalaxy;
    }

    /*
    uint64_t i = 1;
    for(const Position& g: vecGalaxies) {
        cout << i++ << ": {" << g.x << ", " << g.y << "}\n";
    }
    */

    set<Position> donePairs;
    vector<pair<Position, Position>> allGalaxyPairs;
    for(uint64_t i = 0; i < vecGalaxies.size(); i++) {
        for(uint64_t j = 0; j < vecGalaxies.size(); j++) {
            if(i != j) {
                if(donePairs.find(vecGalaxies[j]) == donePairs.end()){
                    allGalaxyPairs.push_back({vecGalaxies[i], vecGalaxies[j]});
                }
            }
        }
        donePairs.insert(vecGalaxies[i]);
    }
    //cout << "Total pairs: " << allGalaxyPairs.size() << "\n";

    uint64_t sumDistances = 0;
    for(const auto& gPair: allGalaxyPairs) {
        sumDistances += getShortestPathManhattan(gPair);
    }
    return sumDistances;
}

int main() {
    // Get input file
    fstream inputFile;
    string file = USE_REAL_DATA ? INPUT_FILE : DEMO_FILE;
    inputFile = fstream(file);
    if(!inputFile.is_open()) {
        cout << "Can't open " << file << endl;
        exit(0);
    }
    
    char* map;
    uint64_t width = 0; uint64_t height = 0;

    vector<uint64_t> rowsExpanded;
    vector<uint64_t> columnsExpanded;

    // Get width and height
    string line;
    getline(inputFile, line);
    width = line.size();
    while(getline(inputFile, line)) {
        height++;
    }
    height++;

    map = new char[width*height];

    inputFile.clear();
    inputFile.seekg(0);
    uint64_t y = 0;
    while(getline(inputFile, line)) {
       for(uint64_t x = 0; x < line.size(); x++) {
            map[x + y*width] = line[x];
       }
       y++;
    }

    printMap(map, width, height);

    // Map Galaxies
    vector<Position> galaxies;
    for(uint64_t y = 0; y < height; y++) {
        for(uint64_t x = 0; x < width; x++) {
            if(map[x + y*width] == '#')galaxies.push_back({x, y});
        }
    }

    getExpansion(map, width, height, rowsExpanded, columnsExpanded);

    cout << "Rows expanded: ";
    for(uint64_t i = 0; i < rowsExpanded.size(); i++) {
        cout << rowsExpanded[i];
        if(i+1 < rowsExpanded.size())cout << ", ";
    }
    cout << "\n";
    cout << "Columns expanded: ";
    for(uint64_t i = 0; i < columnsExpanded.size(); i++) {
        cout << columnsExpanded[i];
        if(i+1 < columnsExpanded.size())cout << ", ";
    }
    cout << "\n";

    cout << "Silver: " << getSumDistance(galaxies, rowsExpanded, columnsExpanded, 2) <<  "\n";
    cout << "Gold: "  << getSumDistance(galaxies, rowsExpanded, columnsExpanded, 1000000) << "\n";

    return 0;
}