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

struct Bound{
    int64_t min;
    int64_t max;
};

struct TotalBounds {
    Bound source;
    Bound destination;
};


struct MapSourceDest {
    Bound source;
    Bound destination;

    void print() const {
        cout << destination.min << " " << source.min << " " << (source.max - source.min) + 1;
    }
};

bool canMapDest(const MapSourceDest& map, int64_t valDest) {
    if(valDest >= map.destination.min && valDest <= map.destination.max)return true;
    return false;
}

bool canMapSource(const MapSourceDest& map, int64_t val) {
    if(val >= map.source.min && val <= map.source.max)return true;
    return false;
}

int64_t mapValue(const MapSourceDest& map, int64_t val) {
    int64_t offset = val - map.source.min;
    return map.destination.min + offset;
}

int64_t mapValueRev(const MapSourceDest& map, int64_t val) {
    int64_t offset = val - map.destination.min;
    return map.source.min + offset;
}

const string MAP_ORDER[] = {"seed-to-soil", "soil-to-fertilizer", "fertilizer-to-water", 
    "water-to-light", "light-to-temperature", "temperature-to-humidity", "humidity-to-location"};

const string MAP_REVERSE[] = {"humidity-to-location", "temperature-to-humidity", "light-to-temperature", "water-to-light", "fertilizer-to-water", 
    "soil-to-fertilizer", "seed-to-soil"};


int64_t findLowest(const unordered_map<string, vector<MapSourceDest>>& maps, int64_t seed) {
    int64_t valMapped = seed;

    //cout << "seed: " << seed << "\n";
    for(const string& mapStr: MAP_ORDER) {
        vector<MapSourceDest> vecMaps = maps.at(mapStr);

        int64_t lowestCanMap = INT64_MAX;
        bool canBeMapped = false;
        for(const MapSourceDest& mapSourceDest: vecMaps) {
            if(canMapSource(mapSourceDest, valMapped)) {
                canBeMapped = true;
                int64_t destValue = mapValue(mapSourceDest, valMapped);
                if(destValue < lowestCanMap)lowestCanMap = destValue;
            }
        }

        if(canBeMapped) {
            //std::cout << "found map, val from: " << valMapped << ", to: " << lowestCanMap << "\n";
            valMapped = lowestCanMap;
        }else{
            //std::cout << "Cant be mapped, val from: " << valMapped << ", to same: " << valMapped << "\n";
        }
        // If it cant be mapped, value goes straight from source to dest
    }

    return valMapped;
}

int64_t findLowestReverse(const unordered_map<string, vector<MapSourceDest>>& maps, int64_t dest) {
    int64_t valMapped = dest;

    //cout << "dest: " << dest << "\n";
    for(const string& mapStr: MAP_REVERSE) {
        vector<MapSourceDest> vecMaps = maps.at(mapStr);

        int64_t lowestCanMap = INT64_MAX;
        bool canBeMapped = false;
        for(const MapSourceDest& mapSourceDest: vecMaps) {
            if(canMapDest(mapSourceDest, valMapped)) {
                canBeMapped = true;
                int64_t destValue = mapValueRev(mapSourceDest, valMapped);
                if(destValue < lowestCanMap)lowestCanMap = destValue;
            }
        }

        if(canBeMapped) {
            //std::cout << "found map, val from: " << valMapped << ", to: " << lowestCanMap << "\n";
            valMapped = lowestCanMap;
        }else{
            //std::cout << "Cant be mapped, val from: " << valMapped << ", to same: " << valMapped << "\n";
        }
        // If it cant be mapped, value goes straight from source to dest
    }

    return valMapped;
}

// get lowest location number
int64_t getSilver(const unordered_map<string, vector<MapSourceDest>>& maps, const vector<int64_t>& seeds) {
    int64_t lowest = INT64_MAX;

    for(int64_t seed: seeds) {
        int64_t numLowestLocation = findLowest(maps, seed);
        if(lowest > numLowestLocation) lowest = numLowestLocation;
    }

    return lowest;
}

int64_t getGoldReverse(const unordered_map<string, vector<MapSourceDest>>& maps, const vector<Bound>& seedRange) {
    for(int64_t dest = 0; dest < 1000000000; dest++) {
        int64_t seed = findLowestReverse(maps, dest);
        for(const Bound& boundsSeeds: seedRange) {
            if(seed >= boundsSeeds.min && seed <= boundsSeeds.max) {
                std::cout << "Gold on value "<< dest << " with seed: " << seed << "\n";
                return seed;
            }
        }
    }

    return INT64_MAX;
}

// (KEK, after reading various explanations later.)
// We want to process all of the ranges, 
int64_t getGold(const unordered_map<string, vector<MapSourceDest>>& maps, const vector<Bound>& seedRange) {
    queue<Bound> beforeMap;
    vector<Bound> afterMap;
    for(const Bound& seedBound: seedRange)beforeMap.push(seedBound);


    for(const string& mapStr: MAP_ORDER) {
        vector<MapSourceDest> vecMaps = maps.at(mapStr);
        
        for(const Bound& d: afterMap)beforeMap.push(d);
        afterMap.clear();

        while(!beforeMap.empty()) {
            Bound rangeToProc = beforeMap.front();
            beforeMap.pop();

            bool hasMapping = false;
            for(const MapSourceDest& map: vecMaps) {
                Bound intersection;
                intersection.min = max(rangeToProc.min, map.source.min); // left side
                intersection.max = min(rangeToProc.max, map.source.max); // Right side
                if(intersection.min < intersection.max) { // Valid range
                    hasMapping = true;
                    // Add the range that coincides to "afterMap" Ad
                    afterMap.push_back({mapValue(map, intersection.min), mapValue(map, intersection.max)});
                    if(intersection.min > rangeToProc.min) {
                        // Insert left segment for processing again
                        beforeMap.push({rangeToProc.min, intersection.min});
                    }
                    if(intersection.max < rangeToProc.max) {
                        // Insert right segment for processing again
                        beforeMap.push({intersection.max, rangeToProc.max});
                    }
                    break;
                }
            }

            // Direct mapping
            if(!hasMapping)afterMap.push_back(rangeToProc);
        }
    }

    int64_t min = INT64_MAX;
    for(const Bound& b: afterMap) min = std::min(min, b.min);

    return min;
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
    
    vector<int64_t> seeds;
    vector<Bound> seedRange;
    unordered_map<string, vector<MapSourceDest>> maps;
    unordered_map<string, TotalBounds> mapBounds;
    

    // Get seeds
    string line;
    getline(inputFile, line);
    vector<string> seedsStr = splitString(splitString(line, "seeds: ")[1], " ");
    for(const string& seed: seedsStr) {
        seeds.push_back(atoll(seed.c_str()));
    }
    for(size_t i = 0; i < seedsStr.size(); i += 2) {
        int64_t min = atoll(seedsStr[i].c_str());
        int64_t max = min + atoll(seedsStr[i+1].c_str()) - 1;
        seedRange.push_back({min, max});
    }

    int64_t maxNumSeed = 0;
    int64_t minNumSeed = INT64_MAX;
    for(const Bound& seedRan: seedRange) {
        if(maxNumSeed < seedRan.max)maxNumSeed = seedRan.max;
        if(minNumSeed > seedRan.min)minNumSeed = seedRan.min;
    }
    cout << "Min num seed: " << minNumSeed << ", Max number of seed: " << maxNumSeed << "\n";
    cout << "Total seeds range to calculate: " << maxNumSeed-minNumSeed << "\n";

    // Get maps
    string actualMap;
    vector<MapSourceDest> mapData;
    while(getline(inputFile, line)) {
        if(!line.empty()) {
            if(line.find("map:") != string::npos) {
                // New map starts
                actualMap = splitString(line, " map:")[0];
                mapData.clear();
            }else{
                // data of maps
                vector<string> val = splitString(line, " ");
                if(val.size() != 3) {
                    cout << "Error, data does not have 3 numbers. Exiting." << "\n";
                    exit(1);
                }

                int64_t des = atoll(val[0].c_str());
                int64_t sou = atoll(val[1].c_str());
                int64_t len = atoll(val[2].c_str());
                Bound source = {sou, sou+len-1};
                Bound dest = {des, des+len-1};

                mapData.push_back({source, dest});
            }
        }else{
            if(!mapData.empty())maps.insert({actualMap,mapData});
        }
    }
    if(!mapData.empty())maps.insert({actualMap,mapData});

   
    
    // Debugging time
    cout << "seeds: ";
    for(int64_t seed: seeds) {
        cout << seed << " ";
    }
    cout << "\n";
    for(const string& mapStr: MAP_ORDER) {
        std::cout << mapStr << " map:\n";
        for(const MapSourceDest& mseed: maps.at(mapStr)) {
            mseed.print();
            cout << "\n";
        } 
    }
    

    // Check map
    for(const string& mapStr: MAP_ORDER) {
        if(maps.find(mapStr) == maps.end()) {
            cout << "Map with string: " << mapStr << ", does not exist in maps. Exiting...\n";
            exit(1);
        }
    }
    

    cout << "Silver: " << getSilver(maps, seeds) << "\n";
    cout << "Gold: "  << getGold(maps, seedRange) << "\n";
    return 0;
}