#include <cctype>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <set>
#include <unordered_set>
#include <queue>
#include <filesystem>
#include <algorithm>

static const char* INPUT_FILE = "input.txt";
static const char* DEMO_FILE = "demo.txt";
static const bool USE_REAL_DATA = true;

// Helpers
std::vector<std::string> splitString(const std::string& str, const std::string& delimiter) {
    std::vector<std::string> strings;

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

void printMap(const char* map, uint32_t width, uint32_t height) {
    for(uint32_t y = 0; y < height; y++) {
        for(uint32_t x = 0; x < width; x++) {
            printf("%c", map[x + width*y]);
        }
        printf("\n");
    }
}

struct Position {
    uint32_t x;
    uint32_t y;
    // compare for order.     
    bool operator <(const Position& pt) const {
        return (x < pt.x) || ((!(pt.x < x)) && (y < pt.y));
    }
};

bool comparePositionNums(const Position& p1, const Position& p2) {
    return p1.x < p2.x;
}


bool isPositionCorrect(const Position& p, uint32_t w, uint32_t h) {
    if(p.x < w && p.y < h)return true;
    return false;
}

uint32_t getSilver(const char* map, uint32_t width, uint32_t height) {
    std::set<Position> numTouchingSymbols;
    // Get all positions of numbers that are touching a symbol
    for(uint32_t y = 0; y < height; y++) {
        for(uint32_t x = 0; x < width; x++) {
            if(!std::isdigit(map[x + width*y]) && map[x + width*y] != '.') {
                for(int l = -1; l <= 1; l++) {
                    for(int u = -1; u <= 1; u++) {
                        Position posSearch = {x+l, y+u};
                        if(isPositionCorrect(posSearch, width, height) && std::isdigit(map[posSearch.x + width*posSearch.y])) {
                            numTouchingSymbols.insert (posSearch);
                        }
                    }
                }
            }
        }
    }

    std::vector<std::vector<Position>> positionsWNumbersLined;

    std::vector<std::string> numberStrs;

    std::queue<Position> numQueue;
    std::set<Position> positionNumsProcessed;
    for(const Position& pos: numTouchingSymbols) {
        
        auto it = positionNumsProcessed.find(pos);
        if(it == positionNumsProcessed.end()) {
            numQueue.push(pos);
        }

        std::vector<Position> posLined;
        while(!numQueue.empty()) {
            Position numPos = numQueue.front();
            numQueue.pop();

            auto it2 = positionNumsProcessed.find(numPos);
            if(it2 == positionNumsProcessed.end()) {
                // if it has not been processed
                positionNumsProcessed.insert(numPos);

                //num += map[numPos.x + numPos.y * width];
                posLined.push_back(numPos);
                // check if there is a number on the left
                Position left = {numPos.x-1, numPos.y};
                if(isPositionCorrect(left, width, height) && std::isdigit(map[left.x + width*left.y])) {
                    numQueue.push(left);
                }
                // check if there is a number on the right
                Position right = {numPos.x+1, numPos.y};
                if(isPositionCorrect(right, width, height) && std::isdigit(map[right.x + width*right.y])) {
                    numQueue.push(right);
                }
            }

        }

        std::sort(posLined.begin(), posLined.end(), comparePositionNums);
        positionsWNumbersLined.push_back(posLined);
        std::string numStr = "";
        for(const Position& p: posLined) {
            numStr += map[p.x + p.y*width]; 
        }
        numberStrs.push_back(numStr);
    }



    uint32_t sum = 0;
    for(const std::string& str: numberStrs) {
        //std::cout << str << "\n";
        sum += std::atoi(str.c_str());
    }
    return sum;
}

std::vector<uint32_t> numbersFromPositions(std::set<Position> posNumbersTouchingGears, const char* map, uint32_t width, uint32_t height) {
    std::vector<uint32_t> posN;

    std::vector<std::vector<Position>> positionsWNumbersLined;

    std::vector<std::string> numberStrs;

    std::queue<Position> numQueue;
    std::set<Position> positionNumsProcessed;
    for(const Position& pos: posNumbersTouchingGears) {
        
        auto it = positionNumsProcessed.find(pos);
        if(it == positionNumsProcessed.end()) {
            numQueue.push(pos);
        }

        std::vector<Position> posLined;
        while(!numQueue.empty()) {
            Position numPos = numQueue.front();
            numQueue.pop();

            auto it2 = positionNumsProcessed.find(numPos);
            if(it2 == positionNumsProcessed.end()) {
                // if it has not been processed
                positionNumsProcessed.insert(numPos);

                //num += map[numPos.x + numPos.y * width];
                posLined.push_back(numPos);
                // check if there is a number on the left
                Position left = {numPos.x-1, numPos.y};
                if(isPositionCorrect(left, width, height) && std::isdigit(map[left.x + width*left.y])) {
                    numQueue.push(left);
                }
                // check if there is a number on the right
                Position right = {numPos.x+1, numPos.y};
                if(isPositionCorrect(right, width, height) && std::isdigit(map[right.x + width*right.y])) {
                    numQueue.push(right);
                }
            }

        }

        std::sort(posLined.begin(), posLined.end(), comparePositionNums);
        positionsWNumbersLined.push_back(posLined);
        std::string numStr = "";
        for(const Position& p: posLined) {
            numStr += map[p.x + p.y*width]; 
        }
        numberStrs.push_back(numStr);
    }

    for(const std::string& str: numberStrs) {
        uint32_t num = std::atoi(str.c_str());
        if(num != 0)posN.push_back(num);
    }
    return posN;
}

uint64_t getGold(const char* map, uint32_t width, uint32_t height) {
    uint64_t gold = 0;

    // Get all positions touching a gear
    for(uint32_t y = 0; y < height; y++) {
        for(uint32_t x = 0; x < width; x++) {
            if(map[x + width*y] == '*') {



                std::set<Position> posNumbersTouchingGears;

                for(int l = -1; l <= 1; l++) {
                    for(int u = -1; u <= 1; u++) {
                        Position posSearch = {x+l, y+u};
                        if(isPositionCorrect(posSearch, width, height) && std::isdigit(map[posSearch.x + width*posSearch.y])) {
                            posNumbersTouchingGears.insert(posSearch); 
                        }
                    }
                }
                std::vector<uint32_t> numsTouching = numbersFromPositions(posNumbersTouchingGears, map, width, height);
                
                if(numsTouching.size() == 2) {
                    gold += numsTouching[0] * numsTouching[1];
                }


            }
        }
    }

    return gold;
}

int main() {
    // Get input file
    std::fstream inputFile;
    std::string file = USE_REAL_DATA ? INPUT_FILE : DEMO_FILE;
    inputFile = std::fstream(file);
    if(!inputFile.is_open()) {
        std::cout << "Can't open " << file << std::endl;
        exit(0);
    }

    char* map;
    uint32_t width = 0;
    uint32_t height = 0;
    
    // Get width and height
    std::string line;
    while(std::getline(inputFile, line)) {
        if(width < line.length())width = line.length();
        height++;
    }

    map = new char[width*height];
    
    // Read map
    inputFile.clear();
    inputFile.seekg(0);
    uint32_t y = 0;
    while(std::getline(inputFile, line)) {
        for(uint32_t x = 0; x < width; x++) {
            map[x + width*y] = line[x];
        }
        y++;
    }

    printMap(map, width, height);


    //std::cout << "Silver: " << getSilver(map, width, height) << "\n";
    std::cout << "Gold: " << getGold(map, width, height) << "\n";
    return 0;
}