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
    uint32_t z;

    bool operator==(const Position& p) const {
        return x == p.x && y == p.y && z == p.z;
    }

    bool operator!=(const Position& p) const {
        return x != p.x || y != p.y || z != p.z;
    }

    bool operator<(const Position& p) const {
        return x < p.x || (x == p.x && y < p.y) || (x == p.x && y == p.y && z < p.z);
    }
};


struct Offset{
    int32_t x;
    int32_t y;
    int32_t z;

    bool operator==(const Offset& p) const {
        return x == p.x && y == p.y && z == p.z;
    }

    bool operator<(const Offset& p) const {
        return x < p.x || (x == p.x && y < p.y) || (x == p.x && y == p.y && z < p.z);
    }
};


//const Offset MOVES[4] = {{0, -1}, {1,0}, {0, 1}, {-1, 0}}; //N, E, S, W

struct Brick {
    Position p1;
    Position p2;
    uint32_t signature;

    bool operator==(const Brick& p) const {
        return p1 == p.p1 && p2 == p.p2;
    }

    bool operator!=(const Brick& p) const {
         return p1 != p.p1 || p2 != p.p2;
    }
};

// We could have a map of cubes to it's respective brick
// So in reality we have a 3d Position that maps to a rectangle
// we can probably order the rectangles by it's lowest Z, then iterating one by one to see where they land
// Then we could BFS from one point of the rectangle to the ground??? We probably need to build a tree or a vector for each piece connected to other
// Maybe do a tree with the ground as a main node, and then go up?

// Custom comparator function
bool compareByHeight(const Brick& lhs, const Brick& rhs) {
    // Compare based on the sum of a, b, and c
    return min(lhs.p1.z, lhs.p2.z) < min(rhs.p2.z, rhs.p2.z);
}

bool doBricksOverlap(const Brick& b1, const Brick& b2) {
    if(b1.p2.x >= b2.p1.x && b1.p1.x <= b2.p2.x &&
       b1.p2.y >= b2.p1.y && b1.p1.y <= b2.p2.y && 
       b1.p2.z >= b2.p1.z && b1.p1.z <= b2.p2.z) {
        return true;
    }
    return false;
}

bool canFall(const Brick& b, const vector<Brick>& bricks) {
    Brick fallingBrick = b;
    fallingBrick.p1.z -= 1;
    fallingBrick.p2.z -= 1;

    bool canFall = true;
    if(fallingBrick.p1.z > 0 && fallingBrick.p2.z > 0) { 
        for(const Brick& br: bricks) {
            if(b != br) {
                if(doBricksOverlap(br, fallingBrick)) {
                    canFall = false;
                    break;
                }
            }
        }
    }else{
        canFall = false;
    }

    return canFall;
}

vector<Brick> dropBricks(const vector<Brick>& bricks) {
    std::vector<Brick> bricksFalling(bricks);

    for(Brick& b: bricksFalling) {
        bool canBrickFall = true;
        while(canBrickFall) {
            if(canFall(b, bricksFalling)) {
                b.p1.z -= 1;
                b.p2.z -= 1;
            }else{
                canBrickFall = false;
            }
        }
    }

    return bricksFalling;
}

uint32_t dropBricksCount(const vector<Brick>& bricks) {
    std::vector<Brick> bricksFalling(bricks);
    uint32_t numBricksFall = 0;


    for(Brick& b: bricksFalling) {
        bool hasFallen = false;
        bool canBrickFall = true;
        while(canBrickFall) {
            if(canFall(b, bricksFalling)) {
                b.p1.z -= 1;
                b.p2.z -= 1;
                hasFallen = true;
            }else{
                canBrickFall = false;
            }
        }
        if(hasFallen)numBricksFall++;
    }

    return numBricksFall;
}

uint32_t getSilver(const vector<Brick>& bricks) {
    uint32_t numberBricksCanRemove = 0;

    for(size_t i = 0; i < bricks.size(); i++) {
        std::vector<Brick> bricksIncomplete(bricks.begin(), bricks.end());
        bricksIncomplete.erase(bricksIncomplete.begin() + i);
        bool canAnyFall = false;
        for(const Brick& b: bricksIncomplete) {
            if(canFall(b, bricksIncomplete)) {
                canAnyFall = true;
                break;
            }
        }
        if(!canAnyFall)numberBricksCanRemove++;
        if(canAnyFall)cout << "Brick " << bricks[i].signature << " cannot be disintegrated safely\n";
    }

    return numberBricksCanRemove;
}

uint32_t getGold(const vector<Brick>& bricks) {
    uint32_t numBricksChainReaction = 0;

    for(size_t i = 0; i < bricks.size(); i++) {
        std::vector<Brick> bricksIncomplete(bricks.begin(), bricks.end());
        bricksIncomplete.erase(bricksIncomplete.begin() + i);
        bool canAnyFall = false;
        for(const Brick& b: bricksIncomplete) {
            if(canFall(b, bricksIncomplete)) {
                canAnyFall = true;
                break;
            }
        }
        if(canAnyFall){
            numBricksChainReaction += dropBricksCount(bricksIncomplete);
        }
    }

    return numBricksChainReaction;
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

    vector<Brick> bricksBeforeFalling;
    uint32_t sign = 0;

    // Get width and height
    std::string line;
    while(std::getline(inputFile, line)) {
        vector<string> brickPos = splitString(line, "~");
        vector<string> p1Str = splitString(brickPos[0], ",");
        vector<string> p2Str = splitString(brickPos[1], ",");
        Position p1 = {(uint32_t)atoi(p1Str[0].c_str()), (uint32_t)atoi(p1Str[1].c_str()), (uint32_t)atoi(p1Str[2].c_str())};
        Position p2 = {(uint32_t)atoi(p2Str[0].c_str()), (uint32_t)atoi(p2Str[1].c_str()), (uint32_t)atoi(p2Str[2].c_str())};
        Position minP = {min(p1.x, p2.x), min(p1.y, p2.y), min(p1.z, p2.z)};
        Position maxP = {max(p1.x, p2.x), max(p1.y, p2.y), max(p1.z, p2.z)};
        bricksBeforeFalling.push_back({minP,maxP, sign});
        sign++;
    }
    
    // 
    cout << "Before sorting: " << "\n";
    for(const auto& b: bricksBeforeFalling) {
        cout << b.signature << ": " << b.p1.x << "," << b.p1.y << "," << b.p1.z << "~" << b.p2.x << "," << b.p2.y << "," << b.p2.z << "\n";
    }

    sort(bricksBeforeFalling.begin(), bricksBeforeFalling.end(), compareByHeight);
    cout << "After sorting: " << "\n";

    for(const auto& b: bricksBeforeFalling) {
        cout << b.signature << ": " << b.p1.x << "," << b.p1.y << "," << b.p1.z << "~" << b.p2.x << "," << b.p2.y << "," << b.p2.z << "\n";
    }  

    cout << "After falling: " << "\n";
    vector<Brick> bricksAfterFalling = dropBricks(bricksBeforeFalling);
    
    for(const auto& b: bricksAfterFalling) {
        cout << b.signature << ": " << b.p1.x << "," << b.p1.y << "," << b.p1.z << "~" << b.p2.x << "," << b.p2.y << "," << b.p2.z << "\n";
    }

    sort(bricksAfterFalling.begin(), bricksAfterFalling.end(), compareByHeight);
    cout << "After sorting: " << "\n";
    for(const auto& b: bricksAfterFalling) {
        cout << b.signature << ": " << b.p1.x << "," << b.p1.y << "," << b.p1.z << "~" << b.p2.x << "," << b.p2.y << "," << b.p2.z << "\n";
    }  

    // Check
    for(const Brick& b: bricksAfterFalling) {
        if(canFall(b, bricksAfterFalling)) {
            cout << "Error, there should not be a single brick that can fall \n";
            return 1; 
        }
    }

    //cout << "Silver: " << getSilver(bricksAfterFalling) << "\n";
    cout << "Gold: " << getGold(bricksAfterFalling) << "\n";
    return 0;
}