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

    bool operator==(const Position& p) const {
        return x == p.x && y == p.y;
    }

    bool operator<(const Position& p) const {
        return x < p.x || (x == p.x && y < p.y);
    }
};

struct Offset{
    int32_t x;
    int32_t y;

    bool operator==(const Offset& p) const {
        return x == p.x && y == p.y;
    }

    bool operator<(const Offset& p) const {
        return x < p.x || (x == p.x && y < p.y);
    }
};

struct Ray {
    Position position;
    Offset movement;

    bool operator==(const Ray& r) const {
        return position == r.position && movement == r.movement;
    }

    bool operator<(const Ray& r) const {
        return position < r.position || (position == r.position && movement < r.movement);
    }
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

    void copyFrom(const GridChar& g) {
        for(uint32_t i = 0; i < g.width*g.height; i++) {
            map[i] = g.map[i];
        }
    }

    bool isInside(uint32_t x, uint32_t y) const {
        if(x < width && y < height)return true;
        return false;
    }

    bool isEqual(const GridChar& grid) const{
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

uint32_t calculateEnergize(const GridChar& grid, Ray ray) {
    uint32_t energized = 0;
    GridChar tilesEnergized = grid.copy();
    

    queue<Ray> rays;
    rays.push(ray);

    set<Ray> raysCalculated;

    while(!rays.empty()) {
        Ray r = rays.front();
        rays.pop();
        
        // Energize tile
        tilesEnergized.map[r.position.x + r.position.y*tilesEnergized.width] = '#';

        // Ray calculated
        raysCalculated.insert(r);

        char tile = grid.map[r.position.x + r.position.y*grid.width];
        // Change movement or spawn new
        if(tile == '.') {
            // do nothing
        }else if(tile == '/') {
            int32_t mX = r.movement.x;
            r.movement.x = -r.movement.y;
            r.movement.y = -mX;
        }else if(tile == '\\') {
            int32_t mX = r.movement.x;
            r.movement.x = r.movement.y;
            r.movement.y = mX;
        }else if(tile == '|') {
            if((r.movement.x == 1 && r.movement.y == 0) 
                || (r.movement.x == -1 && r.movement.y == 0)) {
                // Go up
                r.movement.x = 0;
                r.movement.y = -1;
                // Spawn one that moves down
                Ray nextR = {r.position,{0, 1}};
                if(raysCalculated.find(nextR) == raysCalculated.end()) {
                    rays.push(nextR);
                }
            }
        }else if(tile == '-') {
            if((r.movement.x == 0 && r.movement.y == 1) 
                || (r.movement.x == 0 && r.movement.y == -1)) {
                // Go right
                r.movement.x = 1;
                r.movement.y = 0;
                // Spawn one that moves left
                Ray nextR = {r.position,{-1, 0}};
                if(raysCalculated.find(nextR) == raysCalculated.end()) {
                    rays.push(nextR);
                }
            }
        }

        // Move normally to next tile if it can
        Position next = {r.position.x+r.movement.x, r.position.y+r.movement.y};
        if(grid.isInside(next.x, next.y)) {
            r.position = next;
            if(raysCalculated.find(r) == raysCalculated.end()) {
                rays.push(r);
            }
        } // If we are not inside we don't continue this ray

        //tilesEnergized.print();
        //cout << "\n\n";
    }

    tilesEnergized.print();

    // Count energized
    for(uint32_t i = 0; i < tilesEnergized.width*tilesEnergized.height; i++) {
        if(tilesEnergized.map[i] == '#')energized++;
    }

    return energized;
}

uint32_t getGold(const GridChar& grid) {
    uint32_t mostEnergized = 0;

    for(uint32_t x = 0; x < grid.width; x++) {
        // north
        uint32_t energizedN = calculateEnergize(grid, {{x, 0}, {0, 1}});
        if(mostEnergized < energizedN)mostEnergized = energizedN;

        uint32_t energizedS = calculateEnergize(grid, {{x, grid.height-1}, {0, -1}});
        if(mostEnergized < energizedS)mostEnergized = energizedS;
    }

    for(uint32_t y = 0; y < grid.height; y++) {
        // north
        uint32_t energizedW = calculateEnergize(grid, {{0, y}, {1, 0}});
        if(mostEnergized < energizedW)mostEnergized = energizedW;

        uint32_t energizedE = calculateEnergize(grid, {{grid.width-1, y}, {-1, 0}});
        if(mostEnergized < energizedE)mostEnergized = energizedE;
    }

    return mostEnergized;
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

    cout << "\n";

    cout << "Silver: " << calculateEnergize(grid, {{0,0}, {1, 0}}) << "\n";
    cout << "Gold: " << getGold(grid) << "\n";

    return 0;
}