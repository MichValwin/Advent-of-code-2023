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

uint32_t hashStep(const string& step) {
    uint32_t hash = 0;

    for(char c: step) {
        hash += (uint8_t)c;
        hash *= 17;
        hash = hash % 256;
    }
    
    return hash;
}

uint32_t getSilver(const vector<string>& steps) {
    uint32_t silver = 0;
    for(size_t i = 0; i < steps.size(); i++) {
        uint32_t hash = hashStep(steps[i]);
        cout << steps[i] << " becomes " << hash << "\n";
        silver += hash;
    }
    return silver;
}

void print(const vector<vector<pair<string, uint8_t>>>& hashMap) {
    for(size_t i = 0; i < hashMap.size(); i++) {
        if(!hashMap[i].empty()) {
            cout << "Box " << i << ":"; 
            for(size_t j = 0; j < hashMap[i].size(); j++) {
                cout << "[" << hashMap[i][j].first << " " << (uint32_t)hashMap[i][j].second << "]  ";
            }
            cout << "\n";
        }
    }
}

uint64_t getGold(const vector<string>& steps) {
    uint64_t gold = 0;

    vector<vector<pair<string, uint8_t>>> hashMap(256);

    for(size_t i = 0; i < steps.size(); i++) {
        const string step = steps[i];
        size_t posEqual = step.find("=");
        string label;
        
        // Get label and operation
        if(posEqual != std::string::npos) {
            // has a =
            label = step.substr(0, posEqual);
            uint32_t num = step[posEqual+1] - '0';
            uint32_t hash = hashStep(label);
            // Check if there is a box
            bool foundAndReplaced = false;
            for(std::size_t j = 0; j < hashMap[hash].size(); j++) {
                if(hashMap[hash][j].first == label) {
                    hashMap[hash][j].second = num;
                    foundAndReplaced = true;
                    break;
                }
            }
            // Insert new if not found
            if(!foundAndReplaced)hashMap[hash].push_back({label, num});
        }else{
            // has a -
            size_t posMinus = step.find("-");
            label = step.substr(0, posMinus);
            uint32_t hash = hashStep(label);
            for(std::size_t j = 0; j < hashMap[hash].size(); j++) {
                if(hashMap[hash][j].first == label) {
                    hashMap[hash].erase(hashMap[hash].begin() + j);
                    break;
                }
            }
        }

        // Debug
        cout << "After \"" << step << "\":\n";
        print(hashMap);
    }

    for(size_t i = 0; i < hashMap.size(); i++) {
        if(!hashMap[i].empty()) {
            for(size_t j = 0; j < hashMap[i].size(); j++) {
                gold += (1+i) * (j+1) * hashMap[i][j].second;
            }
        }
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

    vector<string> steps;

    string line;
    std::getline(inputFile, line);
    steps = splitString(line, ",");
    
    for(const string& str: steps) {
        cout << str << "\n";
    }


    //cout << "Silver: " << getSilver(steps) << "\n";
    cout << "Gold: " << getGold(steps) << "\n";

    return 0;
}