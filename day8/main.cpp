#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <vector>
#include <set>
#include <regex>
#include <cmath>
#include <unordered_map>
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

struct MapNode {
    string left;
    string right;
};

uint64_t getStepsToGoZ(const string& start, const string& instructions, const unordered_map<string, MapNode>& mapNodes) {
    uint64_t steps = 0;
    string key = start;
    
    while(key[2] != 'Z') {
        char instr = instructions[steps % instructions.length()];

        if(instr == 'L') {
            key = mapNodes.at(key).left;
        }else if(instr == 'R') {
            key = mapNodes.at(key).right;
        }

        steps++;
    }

    return steps;
}

bool endsInZ(const string& s) {
    return s[s.length()-1] == 'Z';
}

void goLeft(vector<string>& keys, const unordered_map<string, MapNode>& mapNodes) {
    for(string& key: keys) {
        key = mapNodes.at(key).left;
    }
}

void goRight(vector<string>& keys, const unordered_map<string, MapNode>& mapNodes) {
    for(string& key: keys) {
        key = mapNodes.at(key).right;
    }
}

uint64_t getGold(const string& instructions, const unordered_map<string, MapNode>& mapNodes, const vector<string> nodeStartA) {
    vector<string> keys;

    vector<uint64_t> stepsToReachZ(nodeStartA.size());
    for(size_t i = 0; i < nodeStartA.size(); i++) {
        stepsToReachZ[i] = getStepsToGoZ(nodeStartA[i], instructions, mapNodes);
    }

    uint64_t mul = 1;
    for(size_t i = 0; i < stepsToReachZ.size(); i++) {
        mul *= (stepsToReachZ[i]/instructions.length());
    }
    mul *= instructions.length();

    return mul;
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
    
    MapNode nodeStart;
    unordered_map<string, MapNode> mapNodes;
    
    string instructions;
    getline(inputFile, instructions);

    regex patternNodes(R"((...) = \((...), (...)\))");
    std::smatch matches;

    // Get nodes
    string line;
    while(getline(inputFile, line)) {
        if(!line.empty()) {
            if (std::regex_search(line, matches, patternNodes)) {
                if(matches.size() == 4) {
                    string key = matches[1];
                    string val1 = matches[2];
                    string val2 = matches[3];
                    MapNode node = {val1, val2};
                    mapNodes.insert({key, node});
                }else{
                    cout << "Cannot parse line\n";
                    exit(1);
                }
            }
        }
    }

    // Get all nodes ending in A
    vector<string> nodesEndsA;
    for(const auto& node: mapNodes) {
        if(node.first[node.first.length()-1] == 'A')
            nodesEndsA.push_back(node.first);
    }


    cout << "Silver: " << getStepsToGoZ("AAA", instructions, mapNodes) << "\n";
    cout << "Gold: "  << getGold(instructions, mapNodes, nodesEndsA) << "\n";
    return 0;
}