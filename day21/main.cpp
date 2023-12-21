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

const Offset MOVES[4] = {{0, -1}, {1,0}, {0, 1}, {-1, 0}}; //N, E, S, W

uint32_t getSilver(const GridChar& grid, const Position startPoint, uint32_t steps) {
    GridChar g = grid.copy();
    
    //cout << "Calculating " << steps << " steps on a " << grid.width << "x" << grid.height << " grid\n";

    // Put an 'O' in starting pos
    g.map[startPoint.x+startPoint.y*g.width] = 'O';

    for(uint32_t step = 0; step < steps; step++) {
        vector<Position> positions;
        // Get all positions
        for(uint32_t y = 0; y < g.height; y++) {
            for(uint32_t x = 0; x < g.width; x++) {
                if(g.map[x+y*g.width] == 'O') {
                    g.map[x+y*g.width] = '.';
                    positions.push_back({x, y});
                }
            }
        }

        for(const Position& p: positions) {
            for(uint8_t j = 0; j < 4; j++) {
                Position nPos = {p.x+MOVES[j].x, p.y+MOVES[j].y};
                if(g.isInside(nPos.x, nPos.y) && g.map[nPos.x + nPos.y*g.width] == '.') {
                    g.map[nPos.x+nPos.y*g.width] = 'O';
                }
            }
        }

        //if(step % 10000 == 0)cout << (((float)step+1) / ((float)steps+1)) * 100.0 << "%\n";
    }

    // Count
    uint32_t NumO = 0;
    for(uint32_t i = 0; i < g.width*g.height; i++) {
        if(g.map[i] == 'O')NumO++;
    }

    delete[] g.map;
    return NumO;
}

uint64_t getGold(const GridChar& grid, const Position& startPoint) {
    cout << "Grid width: " << grid.width << ", grid height: " << grid.height << "\n";

    cout << "KEK: " << 26501365 / grid.width << "\n";

    uint64_t finalGridWidth = 26501365 / (grid.width) - 1; // IN cells
    cout << "Final grid width: " << finalGridWidth << "\n";

    uint64_t oddGrids = pow((finalGridWidth / 2 * 2 + 1), 2);
    uint64_t evenGrids = pow((finalGridWidth + 1) / 2*2,2);
    cout << "Odd grids: " << oddGrids << ", even grids: " << evenGrids << "\n";

    uint64_t oddPoints = getSilver(grid, startPoint, grid.width*2+1);
    uint64_t evenPoints = getSilver(grid, startPoint, grid.width*2);
    cout << "oddPoints " << oddPoints << ", evenPoins: " << evenPoints << "\n";

    uint64_t gold = oddPoints*oddGrids + evenPoints*evenGrids; // Filled grids

    uint64_t cornerNorth = getSilver(grid, {grid.width-1, startPoint.y}, grid.width-1);
    uint64_t cornerEast = getSilver(grid, {startPoint.x, 0}, grid.width-1);
    uint64_t cornerSouth = getSilver(grid, {0, startPoint.y}, grid.width-1);
    uint64_t cornerWest = getSilver(grid, {startPoint.x, grid.width-1}, grid.width-1);
    
    gold += cornerNorth + cornerEast + cornerSouth + cornerWest;

    // Segments
    uint64_t tinyTrenchNE = getSilver(grid, {grid.width-1, 0}, grid.width / 2 - 1);
    uint64_t tinyTrenchSE = getSilver(grid, {grid.width-1, grid.width-1}, grid.width / 2 - 1);
    uint64_t tinyTrenchNW = getSilver(grid, {0, 0}, grid.width / 2 - 1);
    uint64_t tinyTrenchSW = getSilver(grid, {0, grid.width-1}, grid.width / 2 - 1);

    gold += (tinyTrenchNE + tinyTrenchSE + tinyTrenchNW + tinyTrenchSW)*(finalGridWidth+1);

    uint64_t bigTrenchNE = getSilver(grid, {grid.width-1, 0}, (grid.width*3) / 2 - 1);
    uint64_t bigTrenchSE = getSilver(grid, {grid.width-1, grid.width-1}, (grid.width*3) / 2 - 1);
    uint64_t bigTrenchNW = getSilver(grid, {0, 0}, (grid.width*3) / 2 - 1);
    uint64_t bigTrenchSW = getSilver(grid, {0, grid.width-1}, (grid.width*3) / 2 - 1);

    gold += (bigTrenchNE + bigTrenchSE + bigTrenchNW + bigTrenchSW)*(finalGridWidth);

    cout << "Sol: " << gold << "\n";

    return gold;
}

uint32_t getSilverBFS(const GridChar& grid) {
    // BFS this mf
    set<Position> visited;
    queue<pair<Position, uint32_t>> q; // Pair position, steps

    for(uint32_t y = 0; y < grid.height; y++) {
        for(uint32_t x = 0; x < grid.width; x++) {
            if(grid.map[x+y*grid.width] == 'S') {
                q.push({{x, y}, 0});
                goto gotPosition;
            }
        }
    }

    gotPosition:

    while(!q.empty()) {
        Position p = q.front().first;
        uint32_t steps = q.front().second;
        q.pop();

        for(uint8_t i = 0; i < 4; i++) {
            Position nPos = {p.x+MOVES[i].x, p.y+MOVES[i].y};
            if(grid.isInside(nPos.x, nPos.y) && grid.map[nPos.x + nPos.y*grid.width] == '.') {
                if(visited.find(nPos) == visited.end() && steps < 5) {
                    visited.insert(nPos);
                    q.push({nPos, steps+1});
                }
            }
        }
    }


    // Put positions walked on map for debug
    GridChar g = grid.copy();
    for(const Position& p: visited) {
        g.map[p.x + p.y*g.width] = 'O';
    }
    cout << "\nAfter Walking...\n";
    g.print();

    return visited.size();
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

    uint32_t width = 0;
    uint32_t height = 0;
    GridChar grid;
    Position startingPoint;

    // Get width and height
    std::string line;
    if(std::getline(inputFile, line)) width = line.length();
    while(std::getline(inputFile, line)) {
        height++;
    }
    height++;
 
    grid.width = width;
    grid.height = height;
    grid.map = new char[width*height];

    cout << "Map width: " << width << ", map height: " << height << "\n";

    // Read map
    inputFile.clear();
    inputFile.seekg(0);
    uint32_t y = 0;
    while(std::getline(inputFile, line)) {
        for(uint32_t x = 0; x < grid.width; x++) {
            char c = line[x];
            if(c == 'S') {
                startingPoint = {x, y};
                grid.map[x + y*grid.width] = '.';
            }else{
                grid.map[x + y*grid.width] = c;
            }
            
        }
        y++;
    }

    // triplicate or more map
    /*
    GridChar gridExpanded;
    uint32_t exp = 1;
    gridExpanded.height = height*exp;
    gridExpanded.width = width*exp;

    cout << "Testing map with: " << gridExpanded.width << " width. " << gridExpanded.height << " height\n"; 
    gridExpanded.map = new char[gridExpanded.width*gridExpanded.height];
    Position startPosExpanded = {startingPoint.x*exp+(exp/2), startingPoint.y*exp+(exp/2)};
    // Copy map
    for(uint32_t y = 0; y < gridExpanded.height; y++) {
        for(uint32_t x = 0; x < gridExpanded.width; x++) {
            gridExpanded.map[x+y*gridExpanded.width] = grid.map[x%grid.width + (y%grid.height)*grid.width];
        }
    }
    */

    //gridExpanded.print();

    cout << "Silver: " << getSilver(grid, startingPoint, 64) << "\n";
    cout << "Gold: " <<  getGold(grid, startingPoint) << "\n";

    return 0;
}