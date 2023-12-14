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
#include <cmath>
#include <queue>

using namespace std;

static const char* INPUT_FILE = "input.txt";
static const char* DEMO_FILE = "demo.txt";
static const bool USE_REAL_DATA = true;

// Helpers
vector<std::string> splitString(const string &str, const string &delimiter) {
    vector<std::string> strings;

    int start = 0;
    int end = str.find(delimiter);
    while (end != -1) {
        strings.push_back(str.substr(start, end - start));
        start = end + delimiter.size();
        end = str.find(delimiter, start);
    }
    strings.push_back(str.substr(start, end - start));

    return strings;
}


struct Position {
    uint32_t x;
    uint32_t y;
};

struct Offset{
    int32_t x;
    int32_t y;
};

struct GridChar{
    char* map;
    uint32_t width;
    uint32_t height;

    GridChar copy() const {
        GridChar g;
        g.width = width;
        g.height = height;
        g.map = new char[g.width*g.height];
        for(uint32_t y = 0; y < g.height; y++) {
            for(uint32_t x = 0; x < g.width; x++) {
                g.map[x + y*g.width] = map[x + y*g.width];
            }
        }
        return g;
    }

    bool isInside(uint32_t x, uint32_t y) const {
        if(x < width && y < height)return true;
        return false;
    }

    bool isSomething(uint32_t x, uint32_t y) const {
        if(map[x + y*width] != '.')return true;
        return false;
    }

    bool isEqual(const GridChar& grid) {
        if(grid.width == width && grid.height == height){
            bool sameGrid = true;
            for(uint32_t y = 0; y < height; y++) {
                for(uint32_t x = 0; x < width; x++) {
                    if(grid.map[x + y*width] != map[x + y*width]) {
                        sameGrid = false;
                        goto endLoop;
                    }
                }
            }

            endLoop:
            if(sameGrid) {
                return true;
            }else{
                return false;
            }
        }else{
             return false;
        }
    }

    void print() const {
        for(uint32_t y = 0; y < height; y++) {
            for(uint32_t x = 0; x < width; x++) {
                cout << map[x + y*width];
            }
            cout << "\n";
        }
    }
};

void roll(GridChar& grid, const Offset& offset) {
    bool somethingHasMoved = true;
    while(somethingHasMoved) {
        somethingHasMoved = false;
        for(uint32_t y = 0; y < grid.height; y++) {
            for(uint32_t x = 0; x < grid.width; x++) {
                if(grid.map[x + y*grid.width] == 'O'){
                    Position nPos = {x+offset.x, y+offset.y};
                    if(grid.isInside(nPos.x, nPos.y) && !grid.isSomething(nPos.x, nPos.y)) {
                        // can move
                        somethingHasMoved = true;
                        // change pos
                        grid.map[x + y*grid.width] = '.';
                        grid.map[nPos.x + nPos.y*grid.width] = 'O';
                    }
                }
            }
        }
    }
}

uint32_t getSilver(const GridChar& grid) {
    uint32_t silver = 0;
    // Copy grid
    GridChar g = grid.copy();

    // Roll north
    roll(g, {0, -1});
    // Get load
    for(uint32_t y = 0; y < g.height; y++) {
        for(uint32_t x = 0; x < g.width; x++) {
            if(g.map[x + y*g.width] == 'O') {
                uint32_t row = g.height - y;
                silver += row;
            }
        }
    }

    delete[] g.map;
    return silver;
}

uint32_t getGold(const GridChar& grid) {
    uint32_t gold = 0;
    // Copy grid
    GridChar g = grid.copy();
    const uint32_t TOTAL_CYCLES = 100;

    vector<GridChar> gridCopies;
    gridCopies.push_back(g.copy());

    for(uint32_t i = 0; i < TOTAL_CYCLES; i++) {
        // Roll north
        roll(g, {0, -1});
        // Roll west
        roll(g, {-1, 0});
        // Roll south
        roll(g, {0, 1});
        // Roll east
        roll(g, {1, 0});
        gridCopies.push_back(g.copy());
        if(i % 1000 == 0)cout << i << " cycles done\n";
    }

    vector<pair<uint32_t, uint32_t>> pairsOfIndexGridsThatAreSame;

    for(uint32_t i = 0; i < gridCopies.size(); i++) {
        for(uint32_t j = 1; j < gridCopies.size()-1; j++) {
            if(i != j && gridCopies[i].isEqual(gridCopies[j])){
                pairsOfIndexGridsThatAreSame.push_back({i, j});
            }
        }
    }

    if(!pairsOfIndexGridsThatAreSame.empty()) {
        cout << "There are pairs that are equal\n";
        for(uint32_t i = 0; i < pairsOfIndexGridsThatAreSame.size(); i++) {
            cout << "Pair: " << "{" << pairsOfIndexGridsThatAreSame[i].first << "," << pairsOfIndexGridsThatAreSame[i].second << "}\n";
            if(i % 10 == 0)cout << i << " comparisons done\n";
        }
    }

    


    // Get load
    for(uint32_t y = 0; y < g.height; y++) {
        for(uint32_t x = 0; x < g.width; x++) {
            if(g.map[x + y*g.width] == 'O') {
                uint32_t row = g.height - y;
                gold += row;
            }
        }
    }

    delete[] g.map;
    return gold;
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

    GridChar grid;

    // Get width and height
    uint32_t width = 0;
    uint32_t height = 0;
    std::string line;
    if(std::getline(inputFile, line)) width = line.length();
    while(std::getline(inputFile, line)) {
        height++;
    }
    height++;

    grid.width = width;
    grid.height = height;
    grid.map = new char[grid.width*grid.height];

    // Read map
    inputFile.clear();
    inputFile.seekg(0);
    uint32_t y = 0;
    while(std::getline(inputFile, line)) {
        for(uint32_t x = 0; x < width; x++) {
            grid.map[x + width*y] = line[x];
        }
        y++;
    }

    grid.print();
    /*
    // Roll north
    roll(grid, {0, -1});
    cout << "After moving north\n";
    grid.print();
    */

    cout << "Silver: " << getSilver(grid) <<  "\n";
    cout << "Gold: " << getGold(grid) << "\n";

    return 0;
}