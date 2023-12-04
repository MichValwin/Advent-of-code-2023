#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <regex>
#include <cmath>

static const char* INPUT_FILE = "input.txt";
static const char* DEMO_FILE = "demo.txt";
static const bool USE_REAL_DATA = false;

// Helpers
std::vector<std::string> splitString(const std::string &str, const std::string &delimiter) {
    std::vector<std::string> strings;

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



int main() {
    // Get input file
    std::fstream inputFile;
    std::string file = USE_REAL_DATA ? INPUT_FILE : DEMO_FILE;
    inputFile = std::fstream(file);
    if(!inputFile.is_open()) {
        std::cout << "Can't open " << file << std::endl;
        exit(0);
    }
    

    // Get width and height
    std::string line;
    while(std::getline(inputFile, line)) {
       
    }
    
  

    std::cout << "Silver: "  << "\n";
    std::cout << "Gold: "  << "\n";
    return 0;
}