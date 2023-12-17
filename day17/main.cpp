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

struct Node {
    Position pos;
    Offset move;
    uint32_t heatLoss;
    uint32_t numStepSameDir;
    
    Node* previous;

    Node(Position p, Offset o, uint32_t h, uint32_t numStepsSame, Node* prev) {
        pos = p;
        move = o;
        heatLoss = h;
        numStepSameDir = numStepsSame;
        previous = prev;
    } 

    bool operator==(const Node& n) const {
        return pos == n.pos && move == n.move;
    }

    bool operator<(const Node& n) const {
       return pos < n.pos || (pos == n.pos && move < n.move);
    }
};

struct CompareNodes {
    bool operator()(const Node* l, const Node* r) const {
        if(l->pos != r->pos) return l->pos < r->pos;
        if(l->move != r->move) return l->move < r->move;
        return l->numStepSameDir < r->numStepSameDir;
    }
};

const Offset DIRECTIONS[] = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}}; // N, E, S, W

uint32_t getSilver(const GridChar& grid) {
    uint32_t silver = UINT32_MAX;

    set<Node*, CompareNodes> visited;
    auto compareByHeatLoss = [](const Node* l, const Node* r) {
        return l->heatLoss > r->heatLoss; 
    };
    priority_queue<Node*, vector<Node*>, decltype(compareByHeatLoss)> prioQueue(compareByHeatLoss);

    Node* startNode = new Node({0,0}, {0,0}, 0, 0, nullptr);
    prioQueue.push(startNode);

    const Node* last;
    
    while(!prioQueue.empty()) {
        Node* n = prioQueue.top();
        prioQueue.pop();

        // Skip visited ones
        if(visited.find(n) != visited.end()) {
            continue; 
        }

        visited.insert(n);

        if(n->pos.x == grid.width-1 && n->pos.y == grid.height-1) {
            silver = min(silver, n->heatLoss);
            last = n;
            break;
        }

        // Move
        for(uint8_t i = 0; i < 4; i++) {
            // We cant do 180º 
            if(DIRECTIONS[i].x != -n->move.x || DIRECTIONS[i].y != -n->move.y) {

                if(DIRECTIONS[i] == n->move || (n->move.x == 0 && n->move.y == 0)) {
                    // If we can continue in the same dir
                    if(n->numStepSameDir < 3) {
                        Position nP = {n->pos.x + DIRECTIONS[i].x, n->pos.y + DIRECTIONS[i].y};
                        // Insert next pos
                        if(grid.isInside(nP.x, nP.y)) {
                            uint32_t nextHeatLoss = grid.map[nP.x + nP.y*grid.width] - '0';
                            Node* nn = new Node(nP, DIRECTIONS[i], n->heatLoss+nextHeatLoss, n->numStepSameDir+1, n);
                            if(visited.find(nn) == visited.end())prioQueue.push(nn);
                        }
                    }
                }else{
                    Position nP = {n->pos.x + DIRECTIONS[i].x, n->pos.y + DIRECTIONS[i].y};
                    // Insert next pos
                    if(grid.isInside(nP.x, nP.y)) {
                        uint32_t nextHeatLoss = grid.map[nP.x + nP.y*grid.width] - '0';
                        Node* nn = new Node(nP, DIRECTIONS[i], n->heatLoss+nextHeatLoss, 1, n);
                        if(visited.find(nn) == visited.end())prioQueue.push(nn);
                    }
                }
            }
        }
       
        
    }


    // Print to map
    GridChar g = grid.copy();
    cout << "\n";
    while(last->previous != nullptr) {
        char toPrint = 'O';
        if(last->move == DIRECTIONS[0]){
            toPrint = '^';
        }else if(last->move == DIRECTIONS[1]) {
            toPrint = '>';
        }else if(last->move == DIRECTIONS[2]) {
            toPrint = 'v';
        }else if(last->move == DIRECTIONS[3]) {
            toPrint = '<';
        }
        g.map[last->pos.x + last->pos.y*g.width] = toPrint;
        last = last->previous;
    }
    g.map[last->pos.x + last->pos.y*g.width] = 'O';
    g.print();

    for(Node* n: visited) {
        delete n;
    }
    visited.clear();

    delete[] g.map;
    return silver;
}

uint32_t getGold(const GridChar& grid) {
    uint32_t silver = UINT32_MAX;

    set<Node*, CompareNodes> visited;
    auto compareByHeatLoss = [](const Node* l, const Node* r) {
        return l->heatLoss > r->heatLoss; 
    };
    priority_queue<Node*, vector<Node*>, decltype(compareByHeatLoss)> prioQueue(compareByHeatLoss);

    Node* startNode = new Node({0,0}, {0,0}, 0, 0, nullptr);
    prioQueue.push(startNode);

    const Node* last;
    
    while(!prioQueue.empty()) {
        Node* n = prioQueue.top();
        prioQueue.pop();

        // Skip visited ones
        if(visited.find(n) != visited.end()) {
            continue; 
        }

        visited.insert(n);

        if(n->pos.x == grid.width-1 && n->pos.y == grid.height-1 && n->numStepSameDir >= 4) {
            silver = min(silver, n->heatLoss);
            last = n;
            break;
        }

        if(n->numStepSameDir < 10 && (n->move.x != 0 || n->move.y != 0)) {
            Position nP = {n->pos.x + n->move.x, n->pos.y + n->move.y};
            if(grid.isInside(nP.x, nP.y)) {
                uint32_t nextHeatLoss = grid.map[nP.x + nP.y*grid.width] - '0';
                Node* nn = new Node(nP, n->move, n->heatLoss+nextHeatLoss, n->numStepSameDir+1, n);
                if(visited.find(nn) == visited.end())prioQueue.push(nn);
            }
        }

        if(n->numStepSameDir >= 4 || (n->move.x == 0 && n->move.y == 0)) {
            for(uint8_t i = 0; i < 4; i++) {
                // We cant go straight, that is covered
                if(DIRECTIONS[i].x != n->move.x ||  DIRECTIONS[i].y != n->move.y) {
                    // We cant go backwards
                    if(DIRECTIONS[i].x != -n->move.x || DIRECTIONS[i].y != -n->move.y) {
                        Position nP = {n->pos.x + DIRECTIONS[i].x, n->pos.y + DIRECTIONS[i].y};
                        if(grid.isInside(nP.x, nP.y)) {
                            uint32_t nextHeatLoss = grid.map[nP.x + nP.y*grid.width] - '0';
                            Node* nn = new Node(nP, DIRECTIONS[i], n->heatLoss+nextHeatLoss, 1, n);
                            if(visited.find(nn) == visited.end())prioQueue.push(nn);
                        }
                    }
                }
            }
        }

       
        
    }


    // Print to map
    GridChar g = grid.copy();
    cout << "\n";
    while(last->previous != nullptr) {
        char toPrint = 'O';
        if(last->move == DIRECTIONS[0]){
            toPrint = '^';
        }else if(last->move == DIRECTIONS[1]) {
            toPrint = '>';
        }else if(last->move == DIRECTIONS[2]) {
            toPrint = 'v';
        }else if(last->move == DIRECTIONS[3]) {
            toPrint = '<';
        }
        g.map[last->pos.x + last->pos.y*g.width] = toPrint;
        last = last->previous;
    }
    g.map[last->pos.x + last->pos.y*g.width] = 'O';
    g.print();

    for(Node* n: visited) {
        delete n;
    }
    visited.clear();

    delete[] g.map;
    return silver;
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

    cout << "Silver: " << getSilver(grid) << "\n";
    cout << "Gold: " <<  getGold(grid) << "\n";

    return 0;
}