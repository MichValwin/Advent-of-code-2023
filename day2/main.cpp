#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <cstdlib>

#include <filesystem>

static const char* INPUT_FILE = "input.txt";
static const char* DEMO_FILE = "demo.txt";
static const bool USE_REAL_DATA = true;

// Helpers
struct SplitString{
    std::string delimiter;
    std::vector<std::string> strings;
};

SplitString splitString(const std::string &str, const std::string &delimiter) {
    SplitString strings;
    strings.delimiter = delimiter;

    int start = 0;
    int end = str.find(delimiter);
    while (end != -1) {
        strings.strings.push_back(str.substr(start, end - start));
        start = end + delimiter.size();
        end = str.find(delimiter, start);
    }
    strings.strings.push_back(str.substr(start, end - start));

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

    // Parse
    std::string line;
    while(std::getline(inputFile, line)) {
       // std:atoi probably :P
       // or sscanf
    }

    std::cout << "Silver: " << "\n";

    std::cout << "Gold: " << "\n";
    return 0;
}