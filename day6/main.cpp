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

struct Race {
    uint64_t time;
    uint64_t distance;
};

bool canWin(const Race& race, uint64_t timePressing) {
    int64_t timeRemaining = race.time - timePressing;
    if(timeRemaining <= 0)return false;
    if(timePressing*timeRemaining > race.distance)return true;
    return false;
}

uint64_t getSilver(const vector<Race>& races) {
    uint64_t silver = 0;
    vector<uint64_t> winsTotal;

    for(uint64_t i = 0; i < races.size(); i++) {
        uint64_t wins = 0;
        for(uint64_t timePressing = 1; timePressing < races[i].time-1; timePressing++) {
            if(canWin(races[i], timePressing))wins++;
        }
        winsTotal.push_back(wins);
    }

    silver = winsTotal[0];
    for(uint64_t i = 1; i < winsTotal.size(); i++) {
        silver *= winsTotal[i];
    }
    return silver;
}

uint64_t getGoldSLOW(const Race& race) {
    uint64_t gold = 0;
    vector<uint64_t> winsTotal;

  
    uint64_t wins = 0;
    for(uint64_t timePressing = 1; timePressing < race.time-1; timePressing++) {
        if(canWin(race, timePressing))wins++;
    }
    winsTotal.push_back(wins);
    

    gold = winsTotal[0];
    for(uint64_t i = 1; i < winsTotal.size(); i++) {
        gold *= winsTotal[i];
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
    

    // Get times
    string line;
    getline(inputFile, line);
    vector<string> timesRaces = splitString(splitString(line, "Time:")[1], " ");

    getline(inputFile, line);
    vector<string> distanceRaces = splitString(splitString(line, "Distance:")[1], " ");
    
    std::vector<Race> racesSilver;
    for(const string& time: timesRaces) {
        if(!time.empty())racesSilver.push_back({(uint64_t)atol(time.c_str()), 0});
    }
    int i = 0;
    for(const string& distance: distanceRaces) {
        if(!distance.empty()) {
            racesSilver[i].distance = (uint64_t)atol(distance.c_str());
            i++;
        }
    }

    string timeTotal = "";
    for(const string& time: timesRaces) {
        if(!time.empty())timeTotal += time;
    }
    string distanceTotal = "";
    for(const string& distance: distanceRaces) {
        if(!distance.empty())distanceTotal += distance;
    }
    Race raceGold = {(uint64_t)atol(timeTotal.c_str()), (uint64_t)atol(distanceTotal.c_str())};


    //Debug
    for(uint64_t i = 0; i < racesSilver.size(); i++) {
        cout << "Race " << i+1 << "{ t:" << racesSilver[i].time << ", d:" << racesSilver[i].distance << " }\n"; 
    }
    cout << "Race Gold: " << "{t: " << raceGold.time << ", d: " << raceGold.distance << "}\n"; 


    cout << "Silver: " << getSilver(racesSilver) << "\n";
    cout << "Gold: "  << getGoldSLOW(raceGold) << "\n";
    return 0;
}