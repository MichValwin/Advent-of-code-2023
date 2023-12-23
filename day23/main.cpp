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

struct Position {
    uint32_t x;
    uint32_t y;

    bool operator==(const Position& p) const {
        return x == p.x && y == p.y;
    }

    bool operator!=(const Position& p) const {
        return x != p.x || y != p.y;
    }

    bool operator<(const Position& p) const {
        return x < p.x || (x == p.x && y < p.y);
    }
};

struct PositionHash {
    std::size_t operator()(const Position& p) const {
        return hash<uint32_t>{}(p.x) ^ hash<uint32_t>{}(p.y);
    }
};


struct Offset{
    int32_t x;
    int32_t y;

    bool operator==(const Offset& p) const {
        return x == p.x && y == p.y;
    }

    bool operator!=(const Offset& p) const {
        return x != p.x || y != p.y;
    }

    bool operator<(const Offset& p) const {
        return x < p.x || (x == p.x && y < p.y);
    }
};

struct PositionVis {
    Position p;
    Offset lastMove;
    uint32_t timesVisited;

    bool operator==(const PositionVis& posVis) const {
        return posVis.p == p;
    }

    bool operator!=(const PositionVis& posVis) const {
        return posVis.p != p;
    }

    bool operator<(const PositionVis& posVis) const {
        return posVis.p < p;
    }
};

struct PositionWithFrom {
    Position p;
    Offset lastMove;
    uint32_t timesVisited;

    bool operator==(const PositionVis& posVis) const {
        return posVis.p == p;
    }

    bool operator!=(const PositionVis& posVis) const {
        return posVis.p != p;
    }

    bool operator<(const PositionVis& posVis) const {
        return posVis.p < p;
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

    bool isInside(const Position& p) const {
        if(p.x < width && p.y < height)return true;
        return false;
    }

    char getCharAt(const Position& p) const {
        return map[p.x + p.y*width];
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

uint32_t getSilver(const GridChar& grid, const Position& start, const Position& end) {
    uint32_t longestHike = 0;

    queue<PositionVis> toVisit;

    toVisit.push({start, {0,0}, 0});
    while(!toVisit.empty()) {
        PositionVis posVis = toVisit.front();
        Position p = posVis.p;
        uint32_t steps = posVis.timesVisited;
        Offset lastMoveInv = {-posVis.lastMove.x, -posVis.lastMove.y};
        toVisit.pop();

        if(p == end) {
            if(steps > longestHike) {
                longestHike = steps;
                cout << "Found one path with: " << steps << "\n";
            }
        }

        for(uint8_t i = 0; i < 4; i++) {
            if(lastMoveInv != MOVES[i]) {
                Position nextP = {p.x+MOVES[i].x, p.y+MOVES[i].y};

                if(grid.isInside(nextP) && steps < 10000) {
                    char c = grid.getCharAt(nextP);
                    switch(c) {
                        case '#':
                            // do nothing
                            break;
                        case '.':
                            toVisit.push({nextP, MOVES[i], steps+1});
                            break;
                        case '>':
                            if(i != 3)toVisit.push({nextP, MOVES[i], steps+1});
                            break;
                        case '<':
                             if(i != 1)toVisit.push({nextP, MOVES[i], steps+1});
                            break;
                        case 'v':
                            if(i != 0)toVisit.push({nextP, MOVES[i], steps+1});
                            break;
                    }
                }
            }
        }

    }

    return longestHike;
}

uint32_t LONGEST = 0;
/*
uint32_t getGoldDFSRecursive(const GridChar& grid, const set<PositionVis>& visited, const PositionVis& start, const Position& end) {
    //uint32_t longestHike = 0;

    stack<PositionVis> toVisit;

    set<PositionVis> visitedCopy(visited);

    toVisit.push(start);
    while(!toVisit.empty()) {
        PositionVis posVis = toVisit.top();
        Position p = posVis.p;
        uint32_t steps = posVis.timesVisited;
        Offset lastMoveInv = {-posVis.lastMove.x, -posVis.lastMove.y};
        toVisit.pop();

        visitedCopy.insert(posVis);

        if(p == end) {
            //cout << "Steps: " << steps << "\n";
            return steps;
        } 

        vector<PositionVis> nextValidPositions;

        for(uint8_t i = 0; i < 4; i++) {
            Position nextP = {p.x+MOVES[i].x, p.y+MOVES[i].y};
            if(grid.isInside(nextP)) {
                if(visitedCopy.find({nextP, {0,0}, 0}) == visitedCopy.end()) {
                    // If position is not in visited
                    char c = grid.getCharAt(nextP);
                    switch(c) {
                        case '#':
                            // do nothing
                            break;
                        case '.':
                            nextValidPositions.push_back({nextP, MOVES[i], steps+1});
                            break;
                    }
                }
            }
        }

        if(nextValidPositions.size() == 1) {
            toVisit.push(nextValidPositions[0]);
        }else if(nextValidPositions.size() > 1) {
            for(const PositionVis& nextP: nextValidPositions) {
                uint32_t g = getGoldDFSRecursive(grid, visitedCopy, nextP, end);
                if(g > LONGEST) {
                    cout << "Longest: " << g << "\n";
                    LONGEST = g;
                }
            }
        }
    }

    uint32_t l = LONGEST;
    return l;
}

uint32_t getGold(const GridChar& grid, const Position& start, const Position& end) {
    set<PositionVis> positionsVisited;
    PositionVis startPos = {start, {0,0}, 0};
    return getGoldDFSRecursive(grid, positionsVisited, startPos, end);
}

*/
uint32_t getGoldDFSRecursive2(const unordered_map<Position, vector<pair<Position, uint32_t>>, PositionHash>& connectionsGraph, const set<Position>& visited, const PositionVis& start, const Position& end) {
    uint32_t steps = start.timesVisited;

    set<Position> visitedCopy(visited);

    visitedCopy.insert(start.p);
    
    if(start.p == end) {
        //cout << "Steps: " << steps << "\n";
        return start.timesVisited;
    }

    vector<pair<Position, uint32_t>> nextPositions = connectionsGraph.at(start.p);

    for(size_t i = 0; i < nextPositions.size(); i++) {
        if(visitedCopy.find(nextPositions[i].first) == visitedCopy.end()) {
            uint32_t stepsDoneInDir = getGoldDFSRecursive2(connectionsGraph, visitedCopy, {nextPositions[i].first, {0,0}, steps+nextPositions[i].second}, end);
            if(stepsDoneInDir > steps) {
                
                if(stepsDoneInDir > LONGEST) {
                    LONGEST = stepsDoneInDir;
                    cout << "Longest: " << stepsDoneInDir << "\n";
                }
                
                steps = stepsDoneInDir;
            }
        }
    }

    return 0;
}

uint32_t getGold2(const unordered_map<Position, vector<pair<Position, uint32_t>>, PositionHash>& connectionsGraph, const Position& start, const Position& end) {
    set<Position> positionsVisited;
    PositionVis startPos = {start, {0,0}, 0};
    return getGoldDFSRecursive2(connectionsGraph, positionsVisited, startPos, end);
}


void getConnRec(unordered_map<Position, vector<pair<Position, uint32_t>>, PositionHash>& connections, const GridChar& grid, const Position& from, const Position& end) {
    queue<PositionWithFrom> toVisit;
    
    if(connections.find(from) == connections.end()) {
        connections.insert({from, vector<pair<Position, uint32_t>>()});
    }

    for(uint8_t i = 0; i < 4; i++) {
        Position nextP = {from.x+MOVES[i].x, from.y+MOVES[i].y};
        if(grid.isInside(nextP)) {
            char c = grid.getCharAt(nextP);
            if(c == '.'){
                toVisit.push({nextP, MOVES[i], 1});
            }
        }
    }

    while(!toVisit.empty())  {
        PositionWithFrom pV = toVisit.front();
        toVisit.pop();

        // Add end
        if(pV.p == end) {
            if(connections.find(end) == connections.end()) {
                connections.insert({end, vector<pair<Position, uint32_t>>()});
            }
            connections[from].push_back({end, pV.timesVisited});
            return;
        }

        vector<Position> nextPositions;
        vector<Offset> nextMoves;

        for(uint8_t i = 0; i < 4; i++) {
            Position nextP = {pV.p.x+MOVES[i].x, pV.p.y+MOVES[i].y};
            Offset lastMoveInv = {-pV.lastMove.x, -pV.lastMove.y};
            if(lastMoveInv != MOVES[i]) { // If the next pos is not backwards
                if(grid.isInside(nextP)) {
                    char c = grid.getCharAt(nextP);
                    if(c == '.'){
                        nextPositions.push_back(nextP);
                        nextMoves.push_back((MOVES[i]));
                    }
                }
            }
           
        }

        if(nextPositions.size() == 1) {
            // Normal advance
            toVisit.push({nextPositions[0], nextMoves[0], pV.timesVisited+1});
        }else if(nextPositions.size() > 1) {

            // Add node in which we are if doesn't exist
            if(connections.find(pV.p) == connections.end()) {
                connections.insert({pV.p, vector<pair<Position, uint32_t>>()});
                connections[from].push_back({pV.p, pV.timesVisited}); // make connection back
                // Execute method for every path

                getConnRec(connections, grid, pV.p, end);
            }else{
                connections[from].push_back({pV.p, pV.timesVisited}); // make connection back
            }
        }
    }
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
    Position startingPoint = {1, 0};
   

    // Get width and height
    std::string line;
    if(std::getline(inputFile, line)) width = line.length();
    while(std::getline(inputFile, line)) {
        height++;
    }
    height++;
    
    Position endPosition = {width-2, height-1};

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
            grid.map[x + y*grid.width] = c;
        }
        y++;
    }

    // debug
    grid.print();

    cout << "Silver: " <<  getSilver(grid, startingPoint, endPosition) << "\n";
    
    for(uint32_t y = 0; y < grid.height; y++) {
        for(uint32_t x = 0; x < grid.width; x++) {
            char c = grid.map[x + y*grid.width];
            if(c == '<' || c == '>' || c == 'v'){
                grid.map[x + y*grid.width] = '.';
            }
        }
    }

    unordered_map<Position, vector<pair<Position, uint32_t>>, PositionHash> connectionsGraph;
    getConnRec(connectionsGraph, grid, startingPoint, endPosition);
    for(const auto& conn: connectionsGraph) {
        cout << "Position: {"<< conn.first.x << "," << conn.first.y << "}, connects with: ";
        for(const auto& n: conn.second) {
            cout << "["<< n.first.x << "," << n.first.y << ". " << n.second << "] ";
        }
        cout << "\n";
    }


    cout << "Gold: " <<  getGold2(connectionsGraph, startingPoint, endPosition) << "\n";
    return 0;
}