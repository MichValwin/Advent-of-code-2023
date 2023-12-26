#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
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

struct LineSpring {
    string config;
    vector<uint32_t> groups;
};

LineSpring insertMore(const LineSpring& line, uint32_t quantity) {
    LineSpring ret;
    ret.config = "";
    for(uint32_t i = 0; i < quantity; i++) {
        ret.config += line.config;
        if(i < quantity-1)ret.config += "?";
        ret.groups.insert(ret.groups.end(), line.groups.begin(), line.groups.end());
    }
    return ret;
}

void printVec(const vector<uint32_t>& vec) {
    for(size_t i = 0; i < vec.size(); i++) {
        cout << vec[i];
        if(i < vec.size()-1) cout << ",";
    }
}

vector<uint32_t> lineSpringToInt(const string& line) {
    vector<uint32_t> groups;

    uint32_t brokenRead = 0;
    for(char c: line) {
        if(c == '#'){
            brokenRead++;
        }else{
            if(brokenRead != 0){
                groups.push_back(brokenRead);
                brokenRead = 0;
            }
        }
    }
    if(brokenRead != 0)groups.push_back(brokenRead);

    return groups;
}

string intToComb(uint32_t comb, uint32_t length) {
    string line = "";
    for(uint32_t i = 0; i < length; i++) {
        if((comb >> i & 1) == 1) {
            line += '#';
        }else{
            line += '.';
        }
    }
    return line;
}

bool vecAreSame(const vector<uint32_t>& a, const vector<uint32_t>& b) {
    if(a.size() != b.size())return false;
    for(size_t i = 0; i < a.size(); i++) {
        if(a[i] != b[i])return false;
    }

    return true;
}

string substituteUnkown(const string& entireLine, const string& cToPut) {
    string toRet = entireLine;
    
    uint32_t indexToPut = 0;
    for(size_t i = 0; i < toRet.size(); i++) {
        if(toRet[i] == '?') { 
            toRet[i] = cToPut[indexToPut];
            indexToPut++;
        }
    }

    return toRet;
}

vector<string> getLinesPossibleBrute(const LineSpring& lineSpring) {
    uint32_t numUnkown = 0;
    for(char c: lineSpring.config) {
        if(c=='?')numUnkown++;
    }

    vector<string> possibles;
    for(uint32_t i = 0; i < std::pow(2, numUnkown); i++) {
        string comb = intToComb(i, numUnkown);
        possibles.push_back(substituteUnkown(lineSpring.config, comb));
    }

    return possibles;
}

uint32_t getSilver(const vector<LineSpring>& lineSprings) {
    uint32_t sumPossible = 0;

    for(const LineSpring& lineSpr: lineSprings) {
        // Get all possible lines in string
        vector<string> linesPossible = getLinesPossibleBrute(lineSpr);
        uint32_t numLinesPossible = 0;
        for(const string& l: linesPossible) {
            // Parse line & check if it meets requirement
            vector<uint32_t> groupsNew = lineSpringToInt(l);
            if(vecAreSame(lineSpr.groups, groupsNew))numLinesPossible++;
        }
        cout << lineSpr.config << " - " << numLinesPossible << " arrangements. For: ";
        printVec(lineSpr.groups);
        cout << "\n"; 
        sumPossible += numLinesPossible;
    }

    return sumPossible;
}

struct LineSpringHash {
    size_t operator()(const LineSpring& l) const {
        size_t hash = std::hash<string>{}(l.config);
        for (uint32_t value : l.groups) {
            hash ^= std::hash<uint32_t>{}(value) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        }
        return hash;
    }
};

struct LineSpringEqual {
    bool operator()(const LineSpring& lhs, const LineSpring& rhs) const {
        return lhs.config == rhs.config && lhs.groups == rhs.groups;
    }
};

// Lots of help needed to get at this function LOTS LOTS
uint64_t count(const LineSpring& spring, unordered_map<LineSpring, uint64_t, LineSpringHash, LineSpringEqual>& memo) {
    uint64_t sum = 0;

    if(spring.config.empty()) {
        if(spring.groups.empty()){
            return 1;
        }else{
            //cout << "invalid\n";
            return 0; // INVALID CANT BE NO string without groups
        }
    }

    if(spring.groups.empty()) {
        if(spring.config.find("#") != string::npos) {
            //cout << "invalid\n";
            return 0; // config has more blocks but there is not more groups CANT BE
        }else{
            return 1;
        }
    }

    if(memo.find(spring) != memo.end()) {
        return memo.at(spring);
    }

    // Keep going with the same group if there is not a # character (which indicates the start of a group)
    if(spring.config[0] == '?' || spring.config[0] == '.') {
        string nextConfig = spring.config.substr(1);
        sum += count({nextConfig, spring.groups}, memo);
    }

    // Start of a new group. # is a clear start. ? is a uncertain start but still can be
    if(spring.config[0] == '?' || spring.config[0] == '#') {
        if(spring.groups[0] <= spring.config.size() && // Check if it's possible to fit a block
            spring.config.substr(0, spring.groups[0]).find(".") == string::npos && //A DOT CANNOT BE IN THE GROUP, IT MUST GO AFTER
            (spring.groups[0] == spring.config.size() || spring.config[spring.groups[0]] != '#')) { //
                
                uint32_t l = spring.groups[0]+1;
                if(l > spring.config.size()) 
                    l--;
                string nextConf = spring.config.substr(l);
                vector<uint32_t> nextGroups(spring.groups.begin()+1, spring.groups.end());
                sum += count({nextConf, nextGroups}, memo);
            }
    }

    memo.insert({spring, sum});

    return sum;
}



int64_t countVectorSprings(const vector<LineSpring>& springs) {
    int64_t gold = 0;

    unordered_map<LineSpring, uint64_t, LineSpringHash, LineSpringEqual> memo;

    for(const LineSpring& s: springs) {
        uint64_t arrangements = count(s, memo);
        cout << s.config << " ";
        for(uint32_t i = 0; i < s.groups.size(); i++) {
            cout << s.groups[i];
            if(i < s.groups.size()-1)cout << ",";
        }
        cout << " - " << arrangements << " arrangements\n";
        gold += arrangements;
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

    vector<LineSpring> lineSprings;

    string line;
    while(getline(inputFile, line)) {
        vector<string> split = splitString(line, " ");
        vector<uint32_t> groupsBroken;
        vector<string> groupsBString = splitString(split[1], ",");
        for(const string& n: groupsBString) {
            groupsBroken.push_back(atoi(n.c_str()));
        }
        lineSprings.push_back({split[0], groupsBroken});
    }


    vector<LineSpring> fiveFoldedLineSprings;
    for(const LineSpring& lineSpr: lineSprings) {
        fiveFoldedLineSprings.push_back(insertMore(lineSpr, 5));
    }

    cout << "Silver: " << countVectorSprings(lineSprings) << "\n";
    cout << "Gold: "  <<  countVectorSprings(fiveFoldedLineSprings) << "\n";

    return 0;
}