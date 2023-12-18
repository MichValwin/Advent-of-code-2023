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
    int32_t x;
    int32_t y;

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

struct Dig {
    char direction;
    uint8_t dirIndex;
    uint32_t toDig;
};

const Offset DIRECTION[] = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}}; //R, D, L, U



uint32_t getAreaBFS(const GridChar& grid) {
    uint32_t areaPolygon = 0;

    GridChar g = grid.copy();
    set<Position> seen;
    queue<Position> q;

    q.push({0,0});
    seen.insert({0,0});

    while(!q.empty()) {
        Position p = q.front();
        q.pop();

        // put Char 
        g.map[p.x + p.y*g.width] = '@';

        for(uint32_t i = 0; i < 4; i++) {
            Offset o = DIRECTION[i];
            Position newPos = {p.x+o.x, p.y+o.y};
            if(g.isInside(newPos.x, newPos.y)) {
                if(g.map[newPos.x + newPos.y*g.width] == '.') {
                    if(seen.find(newPos) == seen.end()) {
                        q.push(newPos);
                        seen.insert(newPos);
                    }
                }
            }
        }
        

        //cout << "\n";
        //g.print();
    }

    cout << "\n";
    g.print();
    for(uint32_t i = 0; i < g.width*g.height; i++) {
        if(g.map[i] == '#' || g.map[i] == '.'){
            areaPolygon++;
        }
    }

    delete[] g.map;

    return areaPolygon;
}

uint32_t getSilver(const vector<string> inputLines) {
    vector<Dig> digLines;

    // Get width and height
    std::string line;
    for(const string& line : inputLines) {
        Dig d;
        vector<string> split = splitString(line, " ");
        d.direction = split[0][0];
        switch(d.direction) {
            case 'R':
                d.dirIndex = 0;
                break;
            case 'D':
                d.dirIndex = 1;
                break;
            case 'L':
                d.dirIndex = 2;
                break;
            case 'U':
                d.dirIndex = 3;
                break;
        }
        d.toDig = atoi(split[1].c_str());
        digLines.push_back(d);
    }
    
    for(const Dig& d: digLines) {
        cout << d.direction << " " << d.toDig << "    INDEX: " << (uint32_t)d.dirIndex <<"\n";
        if(d.dirIndex >3) {
            cout << "ERROR\n";
            exit(1);
        }
    }


    Position maxPosition = {INT32_MIN, INT32_MIN};
    Position minPosition = {INT32_MAX, INT32_MAX};

    Position p = {0, 0};
    for(const Dig& d: digLines) {
        Offset o = DIRECTION[d.dirIndex];
        p.x += o.x*d.toDig;
        p.y += o.y*d.toDig;

        maxPosition.x = max(maxPosition.x, p.x);
        maxPosition.y = max(maxPosition.y, p.y);
        minPosition.x = min(minPosition.x, p.x);
        minPosition.y = min(minPosition.y, p.y);
    }
   
    
    GridChar grid;
    // Make a border
    grid.width = (maxPosition.x - minPosition.x)+ 3;
    grid.height = (maxPosition.y - minPosition.y) + 3;
    grid.map = new char[grid.width*grid.height];

    for(uint32_t i = 0; i < grid.width*grid.height; i++)grid.map[i] = '.';

    Position start = {abs(minPosition.x)+1, abs(minPosition.y)+1};
    for(const Dig& d: digLines) {
        Offset o = DIRECTION[d.dirIndex];
        for(uint32_t i = 0; i < d.toDig; i++) {
            start.x += o.x;
            start.y += o.y;
            grid.map[start.x + start.y*grid.width] = '#';
        }
    }

    grid.print();

    cout << "\n";

    return getAreaBFS(grid);
}


int64_t getGold(const vector<string>& lines) {
    vector<Dig> digLines;

    int64_t totalLength = 0;
    int64_t shoelace = 0;
    int64_t sideLength = 0;

    for(const string& line: lines) {
        Dig d;
        string encodedStr = splitString(line, "#")[1].substr(0, 6);
        d.dirIndex = encodedStr[5] - '0';
        string hexStr = encodedStr.substr(0, 5);

            
        size_t p;
        d.toDig = std::stoi(hexStr, &p, 16);
        digLines.push_back(d);

        totalLength += d.toDig;
        

        switch(d.dirIndex) {
            case 0: // Right
                d.direction = 'R';
                sideLength += d.toDig;
                break;
            case 1: // Down
                d.direction = 'D';
                shoelace -= sideLength*d.toDig;
                break;
            case 2: // Left
                d.direction = 'L';
                sideLength -= d.toDig;
                break;
            case 3: // UP
                d.direction = 'U';
                shoelace += sideLength*d.toDig;
                break;
        }
    }

    int64_t gold = abs(shoelace) + totalLength / 2 + 1;

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

    // Get width and height
    std::string line;
    vector<string> lines;
    while(std::getline(inputFile, line)) {
        lines.push_back(line);
    }
    

    cout << "Silver: " << getSilver(lines) << "\n";
    cout << "Gold: " <<  getGold(lines) << "\n";

    return 0;
}