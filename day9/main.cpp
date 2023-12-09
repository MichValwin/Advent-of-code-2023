#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <regex>

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

vector<int64_t> getNextVariations(const vector<int64_t>& nums) {
    vector<int64_t> variations;
    for(size_t i = 1; i < nums.size(); i++) {
        int64_t var = nums[i] - nums[i-1];
        variations.push_back(var);
    }
    return variations;
}

bool isFilledWZeros(const vector<int64_t>& nums) {
    size_t zeros = 0;
    for(int64_t n: nums) {
        if(n == 0) zeros++;
    }
    return nums.size() == zeros;
}

int64_t getNextValue(const vector<int64_t>& nums) {
    vector<vector<int64_t>> lineVariations;
    lineVariations.push_back(nums);

    // Get variations until zeros
    bool reachEnd = false;
    while(!reachEnd) {
        vector<int64_t> currVariations = lineVariations[lineVariations.size()-1];
        vector<int64_t> newVariations = getNextVariations(currVariations);
        
        if(isFilledWZeros(newVariations)){
            reachEnd = true;
        }else{
            lineVariations.push_back(newVariations);
        }
    }

    // Get the last variation number
    const vector<int64_t> lastLine = lineVariations[lineVariations.size()-1];
    int64_t nextValue = lastLine[lastLine.size()-1];

    for(int64_t i = lineVariations.size()-2; i >= 0; i--) {
        vector<int64_t> l = lineVariations[i];
        int64_t lastElem = l[l.size()-1];
        nextValue += lastElem;
    }

    return nextValue;
}

int64_t getPreviousValue(const vector<int64_t>& nums) {
    vector<vector<int64_t>> lineVariations;
    lineVariations.push_back(nums);

    cout << "Getting previous values of: ";
    for(const int64_t n: nums) {
        cout << n << " ";
    }
    

    // Get variations until zeros
    bool reachEnd = false;
    while(!reachEnd) {
        vector<int64_t> currVariations = lineVariations[lineVariations.size()-1];
        vector<int64_t> newVariations = getNextVariations(currVariations);
        
        if(isFilledWZeros(newVariations)){
            reachEnd = true;
        }else{
            lineVariations.push_back(newVariations);
        }
    }

    // Get the previous value
    const vector<int64_t> lastLine = lineVariations[lineVariations.size()-1];
    int64_t previousValue = lastLine[0];

    for(int64_t i = lineVariations.size()-2; i >= 0; i--) {
        vector<int64_t> l = lineVariations[i];
        int64_t firstElem = l[0];
        previousValue = firstElem - previousValue;
    }

    cout << "        Previous value: " << previousValue << "\n";

    return previousValue;
}


int64_t getSilver(const vector<vector<int64_t>>& lineNums) {
    int64_t variationSum = 0;

    for(const vector<int64_t>& nums: lineNums) {
        variationSum += getNextValue(nums);
    }

    return variationSum;
}

int64_t getGold(const vector<vector<int64_t>>& lineNums) {
    int64_t variationSum = 0;

    for(const vector<int64_t>& nums: lineNums) {
        variationSum += getPreviousValue(nums);
    }

    return variationSum;
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
    
    vector<vector<int64_t>> lineNums;

    // Get nodes
    string line;
    while(getline(inputFile, line)) {
        vector<string> numStrs = splitString(line, " ");
        vector<int64_t> nums;
        for(const string& numStr: numStrs)nums.push_back(atoi(numStr.c_str()));
        lineNums.push_back(nums);
    }

    
    //cout << "Silver: " <<  getSilver(lineNums) << "\n";
    cout << "Gold: "  << getGold(lineNums) << "\n";
    return 0;
}