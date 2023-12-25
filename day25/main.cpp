#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <cmath>
#include <queue>
#include <random>

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

bool compareStrings(const std::string& s1, const std::string& s2) {
    return s1 < s2;
}

struct ConnectionWeight {
    string a;
    string b;

    bool operator==(const ConnectionWeight& conn) const {
        if(conn.a == a && conn.b == b) {
            return true;
        }else if(conn.a == b && conn.b == a) {
            return true;
        }
        return false;
    }

   

    // Define operator< for Node
    bool operator<(const ConnectionWeight& n) const {
        // Sort the strings and compare them
        std::string sorted1[] = {a, b};
        std::string sorted2[] = {n.a, n.b};
        std::sort(std::begin(sorted1), std::end(sorted1), compareStrings);
        std::sort(std::begin(sorted2), std::end(sorted2), compareStrings);

        if (sorted1[0] == sorted2[0] && sorted1[1] == sorted2[1]) {
            return false;  // They are equal
        }

        return std::lexicographical_compare(std::begin(sorted1), std::end(sorted1),
                                            std::begin(sorted2), std::end(sorted2));
    }

    
};

/*
uint32_t getSilver(const unordered_map<string, vector<string>>& maps) {
    set<string> visited;
    vector<ConnectionWeight> connectionsW;

    queue<string> toVisit;
    toVisit.push("jqt");

    while(!toVisit.empty()) {
        string module = toVisit.front();
        toVisit.pop();

        visited.insert(module);

        vector<string> connections = maps.at(module);
        for(const string& moduleConnected: connections) {
            ConnectionWeight c;
            if(module < moduleConnected) {
                c = {module, moduleConnected};
            }else{
                c = {moduleConnected, module};
            }

            // check if connection already exists
            bool exist = false;
            for(ConnectionWeight& conn: connectionsW) {
                if(c == conn) {
                    conn.weight++; // add our +1
                    exist = true;
                }
            }

            if(!exist) {
                // Add connection to num passed for that connection
                connectionsW.push_back(c);
            }

            // Go to all next connections that we didn't visited
            if(visited.find(moduleConnected) == visited.end()) {
                toVisit.push(moduleConnected);
            }
        }


    }

    return 0;
}
*/

struct NodeCompare {
    bool operator()(const ConnectionWeight& lhs, const ConnectionWeight& rhs) const {
        return lhs < rhs;
    }
};

void countConnectedPaths(const unordered_map<string, vector<string>>& maps, map<ConnectionWeight, uint32_t, NodeCompare>& mapWeights, const string& start, const string& end) {
    set<string> visited;

    queue<string> toVisit;
    toVisit.push(start);

    visited.insert(start);

    while(!toVisit.empty()) {
        string module = toVisit.front();
        toVisit.pop();

        if(module == end)break;

        //visited.insert(module);

        vector<string> connections = maps.at(module);
        for(const string& moduleConnected: connections) {
            ConnectionWeight c = {module, moduleConnected};

            // Update weights
            if(mapWeights.find(c) == mapWeights.end()) {
                // does not exist
                mapWeights.insert({c, 1});
            }else{
                mapWeights[c]++;
            }

            if(visited.find(moduleConnected) == visited.end()) {
                // if we didn't pass from the connection, visit the node and add it
                toVisit.push(moduleConnected);
                visited.insert(moduleConnected);
            }
        }
    }
}

bool compareSecondValue(const std::pair<ConnectionWeight, uint32_t>& lhs, const std::pair<ConnectionWeight, uint32_t>& rhs) {
    return lhs.second > rhs.second;
}

uint32_t countVisitedNodes(const unordered_map<string, vector<string>>& maps, const string& start) {
    uint32_t visitedCount = 0;
    set<string> visited;

    queue<string> toVisit;
    toVisit.push(start);

    visited.insert(start);

    while(!toVisit.empty()) {
        string module = toVisit.front();
        toVisit.pop();

        visitedCount++;
        //visited.insert(module);

        vector<string> connections = maps.at(module);
        for(const string& moduleConnected: connections) {
            ConnectionWeight c = {module, moduleConnected};

            if(visited.find(moduleConnected) == visited.end()) {
                // if we didn't pass from the connection, visit the node and add it
                toVisit.push(moduleConnected);
                visited.insert(moduleConnected);
            }
        }
    }
    return visitedCount;
}

void removeConnection(unordered_map<string, vector<string>>& map, const ConnectionWeight& connection) {
    vector<string>& connA = map[connection.a]; 
    auto it = remove(connA.begin(), connA.end(), connection.b);
    connA.erase(it);

    vector<string>& connB = map[connection.b]; 
    auto it2 = remove(connB.begin(), connB.end(), connection.a);
    connB.erase(it2);
}

vector<pair<ConnectionWeight, uint32_t>> getMostUsedConnections(const unordered_map<string, vector<string>>& mapNodes, uint32_t count) {
    vector<string> allModules;
    for(const auto& entry: mapNodes) {
        allModules.push_back(entry.first);
    }

    // Get all pairs
    map<ConnectionWeight, uint32_t, NodeCompare> connectionTimesPassed;
    vector<ConnectionWeight> pairsConnections;

    for(uint32_t i = 0; i < allModules.size(); i++) {
        for(uint32_t j = i+1; j < allModules.size(); j++) {
            pairsConnections.push_back({allModules[i], allModules[j]});
        }
    }

    std::random_device rd;
    
    // Create a random number generator using the seed from the random device
    std::mt19937 rng(rd());

    uint32_t pairsToCheck = pairsConnections.size() > 1000 ? 150 : pairsConnections.size();

    // Make 1000 random comparaison
    for(uint32_t i = 0; i < count; i++) {
        for(size_t j = 0; j < pairsToCheck; j++) {
            countConnectedPaths(mapNodes, connectionTimesPassed, pairsConnections[j].a, pairsConnections[j].b);
        }
        // shuffle list
        std::shuffle(pairsConnections.begin(), pairsConnections.end(), rng);
    }

    vector<pair<ConnectionWeight, uint32_t>> connsOrdered;
    for(const auto& entry: connectionTimesPassed) {
        connsOrdered.push_back({entry.first, entry.second});
    }
    sort(connsOrdered.begin(), connsOrdered.end(), compareSecondValue);

    return connsOrdered;
}

uint32_t getSilver2(const unordered_map<string, vector<string>>& maps) {
    uint32_t COUNT = 10;
    

    vector<pair<ConnectionWeight, uint32_t>> mostUsedConnFirst = getMostUsedConnections(maps, COUNT);
    ConnectionWeight a, b, c;
    for(int i = 0; i < 5; i++) {
        a = mostUsedConnFirst[i].first;
        unordered_map<string, vector<string>> mapSlicedAfterFirst(maps);
        removeConnection(mapSlicedAfterFirst, a);
        vector<pair<ConnectionWeight, uint32_t>> mostUsedConnSec = getMostUsedConnections(mapSlicedAfterFirst, COUNT);
        for(int j = 0; j < 10; j++) {
            b = mostUsedConnSec[j].first;
            unordered_map<string, vector<string>> mapSlicedAfterSecond(mapSlicedAfterFirst);
            removeConnection(mapSlicedAfterSecond, b);
            vector<pair<ConnectionWeight, uint32_t>> mostUsedConnThird = getMostUsedConnections(mapSlicedAfterFirst, COUNT);
            for(int k = 0; k < 50; k++) {
                c = mostUsedConnThird[k].first;
                unordered_map<string, vector<string>> mapSlicedAfterThird(mapSlicedAfterSecond);
                removeConnection(mapSlicedAfterThird, c);

                // Count
                uint32_t countNodes = countVisitedNodes(mapSlicedAfterThird, "jqt");
                if(countNodes != maps.size()) {
                    cout << "Nodes counted: " << countNodes << ", from total: " << maps.size() << ". Sol: " << (maps.size() - countNodes) * countNodes << "\n";
                    return (maps.size() - countNodes) * countNodes;
                }else{
                    cout << i <<","<<j<<","<<k<<"\n";
                }
            }
        }
    }

    return 0;
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

    unordered_map<string, vector<string>> maps;

    string line;
    while(std::getline(inputFile, line)) {
        vector<string> keyValue = splitString(line, ": ");
        vector<string> values = splitString(keyValue[1], " ");

        maps.insert({keyValue[0], values});
    }

    // Connect everything
    for(const auto& entry: maps) {
        string key = entry.first;
        for(const string& v: entry.second) {
            if(maps.find(v) != maps.end()) {
                // Search if it has connection to key
               
                bool hasConn = false;
                for(const string& vals: maps.at(v)) {
                    if(key == vals){
                        hasConn = true;
                        break;
                    }
                }
                if(!hasConn) {
                    maps.at(v).push_back(key);
                }
            }else{
                // create entry and add our key
                maps.insert({v, vector<string>()});
                maps.at(v).push_back(key);
            }
        }
    }

     for(const auto& entry: maps) {
        string key = entry.first;
        for(const string& v: entry.second) {
            if(maps.find(v) != maps.end()) {
                // Search if it has connection to key
               
                bool hasConn = false;
                for(const string& vals: maps.at(v)) {
                    if(key == vals){
                        hasConn = true;
                        break;
                    }
                }
                if(!hasConn) {
                    maps.at(v).push_back(key);
                }
            }else{
                // create entry and add our key
                maps.insert({v, vector<string>()});
                maps.at(v).push_back(key);
            }
        }
    }

    // debug
    for(const auto& entry: maps) {
        cout << entry.first << ": ";
        for(const string& v: entry.second) {
            cout << v << " ";
        }
        cout << "\n";
    }


    cout << "Silver: "  << getSilver2(maps) << "\n";
    return 0;
}