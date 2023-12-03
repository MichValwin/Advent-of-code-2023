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

Position getFirstPosNumber(const Position& p, const char* map, uint32_t width, uint32_t height) {
    Position firstPosNum = p;
    Position left =  {firstPosNum.x-1, firstPosNum.y};
    while(isPositionCorrect(left, width, height) && std::isdigit(map[left.x + width*left.y])) {
        firstPosNum = left;
        left =  {firstPosNum.x-1, firstPosNum.y};
    }
    return firstPosNum;
}

bool isPositionOfANumber(const Position& p, const char* map, uint32_t width, uint32_t height) {
    return isPositionCorrect(p, width, height) && std::isdigit(map[p.x + width*p.y]);
}

std::vector<uint32_t> getNumbersFromPositions(std::set<Position> posNumTouching, const char* map, uint32_t width, uint32_t height) {
    std::vector<uint32_t> numResult;

    std::set<Position> positionNumsProcessed;
    for(const Position& pos: posNumTouching) {
        auto it = positionNumsProcessed.find(pos);
        if(it != positionNumsProcessed.end()) {
            continue;
        }

        Position p = getFirstPosNumber(pos, map, width, height);
      
        std::string num = "";
        while(isPositionOfANumber(p, map, width, height)) {
            positionNumsProcessed.insert(p);
            num += map[p.x + width*p.y];
            p.x++;
        }
        numResult.push_back(std::atoi(num.c_str()));
    }

    return numResult;
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

    std::vector<uint32_t> numbersFromPositions = getNumbersFromPositions(numTouchingSymbols, map, width, height);

    uint32_t sum = 0;
    for(uint32_t n: numbersFromPositions) {
        sum += n;
    }
    return sum;
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
                std::vector<uint32_t> numsTouching = getNumbersFromPositions(posNumbersTouchingGears, map, width, height);
                
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


    std::cout << "Silver: " << getSilver(map, width, height) << "\n";
    std::cout << "Gold: " << getGold(map, width, height) << "\n";
    return 0;
}