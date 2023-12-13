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

struct GridChar{
    char* map;
    uint32_t width;
    uint32_t height;

    bool isInside(uint32_t x, uint32_t y) const {
        if(x < width && y < height)return true;
        return false;
    }

    void print() const {
        for(uint32_t y = 0; y < height; y++) {
            for(uint32_t x = 0; x < width; x++) {
                cout << map[x + y*width];
            }
            cout << "\n";
        }
    }

    bool areRowsEquals(uint32_t y1, uint32_t y2) const{
        for(uint32_t x = 0; x < width; x++) {
            if(map[x + y1*width] != map[x +y2*width])return false;
        }
        return true;
    }

    bool areColumnsEquals(uint32_t x1, uint32_t x2) const{
        for(uint32_t y = 0; y < height; y++) {
            if(map[x1 + y*width] != map[x2 +y*width])return false;
        }
        return true;
    }

    uint32_t errorsPerRow(uint32_t y1, uint32_t y2) const{
        uint32_t errors = 0;
        for(uint32_t x = 0; x < width; x++) {
            if(map[x + y1*width] != map[x +y2*width])errors++;
        }
        return errors;
    }

    uint32_t errorsPerColumn(uint32_t x1, uint32_t x2) const{
        uint32_t errors = 0;
        for(uint32_t y = 0; y < height; y++) {
            char c1 = map[x1 + y*width];
            char c2 = map[x2 + y*width];
            if(c1 != c2)errors++;
        }
        return errors;
    }

    vector<Position> getSmudgeRows(uint32_t y1, uint32_t y2) const{
        vector<Position> smudges;
        uint32_t errors = 0;
        uint32_t xDiff;
        for(uint32_t x = 0; x < width && errors < 2; x++) {
           if(map[x + y1*width] != map[x +y2*width]) {
                xDiff = x;
                errors++;
            }
        }
        if(errors == 1) {
            smudges.push_back({xDiff, y1});
            smudges.push_back({xDiff, y2});
        }

        return smudges;
    }

    vector<Position> getSmudgeCols(uint32_t x1, uint32_t x2) const{
        vector<Position> smudges;
        uint32_t errors = 0;
        uint32_t yDiff;
         for(uint32_t y = 0; y < height && errors < 2; y++) {
           if(map[x1 + y*width] != map[x2 +y*width]) {
                yDiff = y;
                errors++;
            }
        }
        if(errors == 1) {
            smudges.push_back({x1, yDiff});
            smudges.push_back({x2, yDiff});
        }

        return smudges;
    }

    void changePos(const Position& p) {
        cout << "Changing {" << p.x << "," <<p.y << "}\n";
        if(map[p.x + p.y*width] == '.') {
            map[p.x + p.y*width] = '#';
        }else{
            map[p.x + p.y*width] = '.';
        }
    }
};

struct Reflection {
    vector<pair<uint32_t, uint32_t>> reflectionsHor;
    vector<pair<uint32_t, uint32_t>> reflectionsVert;
};

vector<pair<uint32_t, uint32_t>> getAllColsDup(const GridChar& grid) {
    uint32_t c1 = 0;
    uint32_t c2 = c1+1;

    vector<pair<uint32_t, uint32_t>> pairs;

    while(c2 < grid.width) {
        if(grid.areColumnsEquals(c1, c2)) {
            pairs.push_back({c1, c2});
        }
        c1++;
        c2++;
    }
    return pairs;
}

vector<pair<uint32_t, uint32_t>> getAllRowsDup(const GridChar& grid) {
    uint32_t r1 = 0;
    uint32_t r2 = r1+1;
    vector<pair<uint32_t, uint32_t>> pairs;
    while(r2 < grid.height) {
        if(grid.areRowsEquals(r1, r2)) {
            pairs.push_back({r1, r2});
        }
        r1++;
        r2++;
    }
    return pairs;
}

vector<pair<uint32_t, uint32_t>> getAllRowsDupByOne(const GridChar& grid) {
    uint32_t r1 = 0;
    uint32_t r2 = r1+1;
    vector<pair<uint32_t, uint32_t>> pairs;
    while(r2 < grid.height) {
        if(grid.errorsPerRow(r1, r2) == 1){
            pairs.push_back({r1, r2});
        }
        r1++;
        r2++;
    }
    return pairs;
}

vector<pair<uint32_t, uint32_t>> getAllColsDupByOne(const GridChar& grid) {
    uint32_t c1 = 0;
    uint32_t c2 = c1+1;

    vector<pair<uint32_t, uint32_t>> pairs;

    while(c2 < grid.width) {
        if(grid.errorsPerColumn(c1, c2) == 1) {
            pairs.push_back({c1, c2});
        }
        c1++;
        c2++;
    }
    return pairs;
}

bool isReflectionVert(const GridChar& grid, uint32_t c1, uint32_t c2) {
    bool hasReflection = true;
    while(c1 != UINT32_MAX && c2 < grid.width) {
        if(!grid.areColumnsEquals(c1, c2)){
            hasReflection = false;
            break;
        }
        c1--;
        c2++;
    }
    return hasReflection;
}

bool isReflectionHor(const GridChar& grid, uint32_t r1, uint32_t r2) {
    bool hasReflection = true;
    while(r1 != UINT32_MAX && r2 < grid.height) {
        if(!grid.areRowsEquals(r1, r2)) {
            hasReflection = false;
            break;
        }
        r1--;
        r2++;
    }

    return hasReflection;
}

bool isReflectionVertByOne(const GridChar& grid, uint32_t c1, uint32_t c2, vector<Position>& smudgePos) {
    uint32_t errors = 0;
    bool found = false;


    while(c1 != UINT32_MAX && c2 < grid.width) {
        uint32_t errorsInColumn = grid.errorsPerColumn(c1, c2);
        errors += errorsInColumn;
        if(errors == 1 && !found){
            smudgePos = grid.getSmudgeCols(c1, c2);
            found = true;
        }

        c1--;
        c2++;
    }

    return errors == 1;
}


bool isReflectionHorByOne(const GridChar& grid, uint32_t r1, uint32_t r2, vector<Position>& smudgePos) {
    uint32_t errors = 0;
    bool found = false;

    while(r1 != UINT32_MAX && r2 < grid.height) {
        uint32_t errorsInRow = grid.errorsPerRow(r1, r2);
        errors += errorsInRow;
        if(errors == 1 && !found){
            smudgePos = grid.getSmudgeRows(r1, r2);
            found = true;
        }
       
        r1--;
        r2++;
    }

    return errors == 1;
}

vector<pair<uint32_t, uint32_t>> getReflectionsHor(const GridChar& grid) {
    vector<pair<uint32_t, uint32_t>> pairsDupHor = getAllRowsDup(grid);
    vector<pair<uint32_t, uint32_t>> reflections;
    for(auto& pairDup: pairsDupHor) {
        uint32_t r1 = pairDup.first;
        uint32_t r2 = pairDup.second;
        if(isReflectionHor(grid, r1, r2)) {
            reflections.push_back({r1, r2});
        }
    }
    return reflections;
}

Position getSmudPosRow(const GridChar& grid, uint32_t r1, uint32_t r2) {
    for(uint32_t x = 0; x < grid.width; x++) {
        if(grid.map[x + r1*grid.width] != grid.map[x + r2*grid.width])return {x, r1};
    }
    cout << "ERROR, nor found when it should one error\n";
    return {UINT32_MAX,UINT32_MAX};
}

Position getSmudPosCol(const GridChar& grid, uint32_t c1, uint32_t c2) {
    for(uint32_t y = 0; y < grid.height; y++) {
        if(grid.map[c1 + y*grid.width] != grid.map[c2 + y*grid.width])return {c1, y};
    }
    cout << "ERROR, nor found when it should one error\n";
    return {UINT32_MAX,UINT32_MAX};
}

uint32_t findSmudge(GridChar& grid) {
    uint32_t goldq = 0;
    vector<pair<uint32_t, uint32_t>> pairsDupHor = getAllRowsDup(grid);
    vector<pair<uint32_t, uint32_t>> pairsDupCols = getAllColsDup(grid);
    
    vector<Position> horSmudges;
    for(auto& pairDup: pairsDupHor) {
        vector<Position> p;
        bool isReflectHorByOne = isReflectionHorByOne(grid, pairDup.first, pairDup.second, p);
        if(isReflectHorByOne) {
            goldq += pairDup.second*100;
            horSmudges.insert(horSmudges.end(), p.begin(), p.end());
            //cout << "By one\n";
        }
    }

    vector<Position> vertSmudges;
    for(auto& pairDup: pairsDupCols) {
        vector<Position> p;
        bool isReflectVertByOne = isReflectionVertByOne(grid, pairDup.first, pairDup.second, p);
         if(isReflectVertByOne) {
            goldq += pairDup.second;
            vertSmudges.insert(vertSmudges.end(), p.begin(), p.end());
        }
    }

    if(horSmudges.empty() && vertSmudges.empty()) {
        cout << "No smudges Found" << "\n";
        vector<pair<uint32_t, uint32_t>> pairHorsDByOne = getAllColsDupByOne(grid); // x1, x2
        vector<pair<uint32_t, uint32_t>> pairVertDByOne = getAllRowsDupByOne(grid); // y1, y2

        if(pairHorsDByOne.empty() && pairVertDByOne.empty()) {
            cout << "NO SMUDGES FOUND IN PAIR BY PAIR\n";
            grid.print();
        }

        if(!pairVertDByOne.empty()) {
            Position p = getSmudPosRow(grid, pairVertDByOne[0].first, pairVertDByOne[0].second);
            goldq += pairVertDByOne[0].second*100;
            vertSmudges.push_back(p);
        }

        if(!pairHorsDByOne.empty()){
            Position p = getSmudPosCol(grid, pairHorsDByOne[0].first, pairHorsDByOne[0].second);
            goldq += pairHorsDByOne[0].second;
            vertSmudges.push_back(p);
        }
    }

    if(horSmudges.size() > 0 && vertSmudges.size() > 0) {
        cout << "Smudges found both by hor and vert\n";
    }

    if(horSmudges.size()== 0 && vertSmudges.size() == 0) {
        cout << "Smudges NOT FOUND NEIGHER\n";
    }

    cout << "Size horSmudges: " << horSmudges.size() << ", Size vert smudges: " << vertSmudges.size() << "\n";
    // Correct the thing
    if(horSmudges.size() > 0) {
        Position p = horSmudges[0];
        grid.changePos(p);
    }

    if(vertSmudges.size() > 0) {
        Position p = vertSmudges[0];
        grid.changePos(p);
    }

    cout << "REal gold?: " << goldq << "\n";

    return goldq;
}

uint32_t findSmudge2(GridChar& grid) {
    uint32_t gold = 0;

    // Check horizontal
    uint32_t r1 = 0;
    uint32_t r2 = 1;
    while(r2 < grid.height) {
        vector<Position> p;
        bool isReflectHorByOne = isReflectionHorByOne(grid, r1, r2, p);
        if(isReflectHorByOne) {
            cout << "Found smudge horizontal\n";
            gold += r2*100;
        }
        r1++;
        r2++;
    }

    // Check Vertical
    uint32_t c1 = 0;
    uint32_t c2 = 1;
    while(c2 < grid.width) {
        vector<Position> p;
        bool isReflectHorByOne = isReflectionVertByOne(grid, c1, c2, p);
        if(isReflectHorByOne) {
            cout << "Found smudge Vertical\n";
            gold += c2;
        }
        c1++;
        c2++;
    }

    return gold;
}

vector<pair<uint32_t, uint32_t>>  getReflectionsVert(const GridChar& grid) {
    vector<pair<uint32_t, uint32_t>> pairsDupCols = getAllColsDup(grid);
    vector<pair<uint32_t, uint32_t>> reflections;
    for(auto& pairDup: pairsDupCols) {
        uint32_t c1 = pairDup.first;
        uint32_t c2 = pairDup.second;
        if(isReflectionVert(grid, c1, c2)) {
            reflections.push_back({c1, c2});
        }
    }
    return reflections;
}


uint64_t getSilver(const vector<GridChar>& reflections) {
    uint64_t silver = 0;

    for(uint32_t i = 0; i < reflections.size(); i++) {
        vector<pair<uint32_t, uint32_t>> reflectionsHor = getReflectionsHor(reflections[i]);
        vector<pair<uint32_t, uint32_t>> reflectionsVert = getReflectionsVert(reflections[i]);
        for(const auto& pHor: reflectionsHor) {
            silver += pHor.second*100;
        }
        for(const auto& pVert: reflectionsVert) {
            silver += pVert.second;
        }
    }
    
    return silver;
}

uint64_t getGold(const vector<Reflection> old, const vector<GridChar>& reflections) {
    uint64_t gold = 0;
    
    for(uint32_t i = 0; i < reflections.size(); i++) {
        vector<pair<uint32_t, uint32_t>> reflectionsHorNEW = getReflectionsHor(reflections[i]);
        vector<pair<uint32_t, uint32_t>> reflectionsVertNEW = getReflectionsVert(reflections[i]);

        vector<pair<uint32_t, uint32_t>> reflectionsHorOLD = old[i].reflectionsHor;
        vector<pair<uint32_t, uint32_t>> reflectionsVertOLD = old[i].reflectionsVert;

        for(const auto& pHor: reflectionsHorNEW) {
            if(find(reflectionsHorOLD.begin(), reflectionsHorOLD.end(), pHor) == reflectionsHorOLD.end()){
                gold += pHor.second*100;
            }
            
        }
        for(const auto& pVert: reflectionsVertNEW) {
            if(find(reflectionsVertOLD.begin(), reflectionsVertOLD.end(), pVert) == reflectionsVertOLD.end()){
                gold += pVert.second;
            }
           
        }
    }

    if(gold == 0) {
        cout << "Err, not found smudge\n";
    }

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

    vector<GridChar> reflections;

    // Parsing
    vector<vector<string>> refl;
    vector<string> cs;
    string line;
    while(getline(inputFile, line)) {
        if(line.empty()){
            refl.push_back(cs);
            cs.clear();
        }else{
            cs.push_back(line);
        }
    }
    refl.push_back(cs);
    cs.clear();

    for(auto& vecS: refl) {
        uint32_t width = vecS[0].size();
        uint32_t height = vecS.size();
        char* grid = new char[width*height];
        uint32_t y = 0;
        for(const string& s: vecS) {
            for(uint32_t x = 0; x < s.size(); x++) {
                grid[x + y*width] = s[x];
            }
            y++;
        }

        reflections.push_back({grid, width, height});
    }

    // Debug
    for(const GridChar& grid: reflections) {
        grid.print();
        cout << "\n";
    }

    /*
    for(uint32_t i = 0; i < reflections.size(); i++) {
        vector<pair<uint32_t, uint32_t>> reflectionsHor = getReflectionsHor(reflections[i]);
        vector<pair<uint32_t, uint32_t>> reflectionsVert = getReflectionsVert(reflections[i]);
        cout << (i+1) << "º " << "has Hor: " << reflectionsHor.size() << ", and Vert: " << reflectionsVert.size() << "\n";
        if(!reflectionsHor.empty()){

        }
        if(!reflectionsVert.empty()) {
            cout << "Reflections vert in: ";
            for(const auto& reflVert: reflectionsVert) {
                cout << "{" << reflVert.first << "," << reflVert.second << "} ";
            }
            cout << "\n";
        }
    }
    */

    vector<Reflection> reflectionsOld;

    for(uint32_t i = 0; i < reflections.size(); i++) {
        vector<pair<uint32_t, uint32_t>> reflectionsHor = getReflectionsHor(reflections[i]);
        vector<pair<uint32_t, uint32_t>> reflectionsVert = getReflectionsVert(reflections[i]);
        reflectionsOld.push_back({reflectionsHor, reflectionsVert});
    }

    cout << "Silver: " << getSilver(reflections) << "\n";

    uint32_t g = 0;
    for(uint32_t i = 0; i < reflections.size(); i++) {
        g += findSmudge2(reflections[i]);
    }   
    cout << "GOLD total? << " << g << "\n";
    /*
    cout << "\n";
    for(const GridChar& grid: reflections) {
        grid.print();
        cout << "\n";
    }
    */




    //cout << "Silver: " << getSilver(reflections) << "\n";
    //cout << "Gold: " << getGold(reflectionsOld, reflections) << "\n";

    return 0;
}