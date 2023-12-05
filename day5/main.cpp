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

struct Bounds{
    uint64_t min;
    uint64_t max;
};

struct TotalBounds {
    Bounds source;
    Bounds destination;
};


struct MapSourceDest {
    Bounds source;
    Bounds destination;

    void print() const {
        cout << destination.min << " " << source.min << " " << (source.max - source.min) + 1;
    }
};

bool canMapDest(const MapSourceDest& map, uint64_t valDest) {
    if(valDest >= map.destination.min && valDest <= map.destination.max)return true;
    return false;
}

bool canMapSource(const MapSourceDest& map, uint64_t val) {
    if(val >= map.source.min && val <= map.source.max)return true;
    return false;
}

uint64_t mapValue(const MapSourceDest& map, uint64_t val) {
    uint64_t offset = val - map.source.min;
    return map.destination.min + offset;
}

uint64_t mapValueRev(const MapSourceDest& map, uint64_t val) {
    uint64_t offset = val - map.destination.min;
    return map.source.min + offset;
}

const string MAP_ORDER[] = {"seed-to-soil", "soil-to-fertilizer", "fertilizer-to-water", 
    "water-to-light", "light-to-temperature", "temperature-to-humidity", "humidity-to-location"};

const string MAP_REVERSE[] = {"humidity-to-location", "temperature-to-humidity", "light-to-temperature", "water-to-light", "fertilizer-to-water", 
    "soil-to-fertilizer", "seed-to-soil"};


uint64_t findLowest(const unordered_map<string, vector<MapSourceDest>>& maps, uint64_t seed) {
    uint64_t valMapped = seed;

    //cout << "seed: " << seed << "\n";
    for(const string& mapStr: MAP_ORDER) {
        vector<MapSourceDest> vecMaps = maps.at(mapStr);

        uint64_t lowestCanMap = UINT64_MAX;
        bool canBeMapped = false;
        for(const MapSourceDest& mapSourceDest: vecMaps) {
            if(canMapSource(mapSourceDest, valMapped)) {
                canBeMapped = true;
                uint64_t destValue = mapValue(mapSourceDest, valMapped);
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

uint64_t findLowestReverse(const unordered_map<string, vector<MapSourceDest>>& maps, uint64_t dest) {
    uint64_t valMapped = dest;

    //cout << "dest: " << dest << "\n";
    for(const string& mapStr: MAP_REVERSE) {
        vector<MapSourceDest> vecMaps = maps.at(mapStr);

        uint64_t lowestCanMap = UINT64_MAX;
        bool canBeMapped = false;
        for(const MapSourceDest& mapSourceDest: vecMaps) {
            if(canMapDest(mapSourceDest, valMapped)) {
                canBeMapped = true;
                uint64_t destValue = mapValueRev(mapSourceDest, valMapped);
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
uint64_t getSilver(const unordered_map<string, vector<MapSourceDest>>& maps, const vector<uint64_t>& seeds) {
    uint64_t lowest = UINT64_MAX;

    for(uint64_t seed: seeds) {
        uint64_t numLowestLocation = findLowest(maps, seed);
        if(lowest > numLowestLocation) lowest = numLowestLocation;
    }

    return lowest;
}

uint64_t getGoldReverse(const unordered_map<string, vector<MapSourceDest>>& maps, const vector<Bounds>& seedRange) {
    for(uint64_t dest = 0; dest < 1000000000; dest++) {
        uint64_t seed = findLowestReverse(maps, dest);
        for(const Bounds& boundsSeeds: seedRange) {
            if(seed >= boundsSeeds.min && seed <= boundsSeeds.max) {
                std::cout << "Gold on value "<< dest << " with seed: " << seed << "\n";
                return seed;
            }
        }
    }

    return UINT64_MAX;
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
    
    vector<uint64_t> seeds;
    vector<Bounds> seedRange;
    unordered_map<string, vector<MapSourceDest>> maps;
    unordered_map<string, TotalBounds> mapBounds;
    

    // Get seeds
    string line;
    getline(inputFile, line);
    vector<string> seedsStr = splitString(splitString(line, "seeds: ")[1], " ");
    for(const string& seed: seedsStr) {
        seeds.push_back(atoll(seed.c_str()));
    }
    for(uint64_t i = 0; i < seedsStr.size(); i += 2) {
        uint64_t min = atoll(seedsStr[i].c_str());
        uint64_t max = min + atoll(seedsStr[i+1].c_str()) - 1;
        seedRange.push_back({min, max});
    }

    uint64_t maxNumSeed = 0;
    uint64_t minNumSeed = UINT64_MAX;
    for(const Bounds& seedRan: seedRange) {
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

                uint64_t des = atoll(val[0].c_str());
                uint64_t sou = atoll(val[1].c_str());
                uint64_t len = atoll(val[2].c_str());
                Bounds source = {sou, sou+len-1};
                Bounds dest = {des, des+len-1};

                mapData.push_back({source, dest});
            }
        }else{
            if(!mapData.empty())maps.insert({actualMap,mapData});
        }
    }
    if(!mapData.empty())maps.insert({actualMap,mapData});

   
    
    // Debugging time
    
    cout << "seeds: ";
    for(uint64_t seed: seeds) {
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

    // Get maps  total source and dest bounds
    for(const auto& map: maps) {
        const string mapID =  map.first;
        Bounds sourceBounds = {UINT64_MAX-1, 0};
        Bounds destBounds = {UINT64_MAX-1, 0};

        for(const MapSourceDest& mapSourceDes: map.second) {
            if(mapSourceDes.source.min < sourceBounds.min)sourceBounds.min = mapSourceDes.source.min;
            if(mapSourceDes.source.max > sourceBounds.max)sourceBounds.max = mapSourceDes.source.max;

            if(mapSourceDes.destination.min < destBounds.min)destBounds.min = mapSourceDes.destination.min;
            if(mapSourceDes.destination.max > destBounds.max)destBounds.max = mapSourceDes.destination.max;
        }
        mapBounds.insert({mapID, {sourceBounds, destBounds}});
    }

    for(const auto& mapTotBounds: mapBounds) {
        cout << "Map " << mapTotBounds.first << ", bounds source: " << mapTotBounds.second.source.min << "," << mapTotBounds.second.source.max <<
            ". bounds Dest: " << mapTotBounds.second.destination.min << "," << mapTotBounds.second.destination.max << "\n";
    }
    
    /*
    if(findLowest(maps, 79) != 82 || findLowest(maps, 14) != 43
         || findLowest(maps, 55) != 86 || findLowest(maps, 13) != 35) {
        std::cout << "err\n";
        exit(1);
    }
    */
    

    cout << "Silver: " << getSilver(maps, seeds) << "\n";
    cout << "Gold: "  << getGoldReverse(maps, seedRange) << "\n";
    return 0;
}