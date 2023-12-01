#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cctype>

static const char* INPUT_FILE = "input.txt";
static const char* DEMO_FILE = "demo.txt";
static const bool USE_REAL_DATA = true;

uint32_t getSilver(std::vector<std::string>& lines) {
    uint32_t result = 0;
    for(std::string& line: lines) {
        char first = 0;
        char last = 0;
        for(char d: line) {
            if(std::isdigit(d)) {
                if(first == 0)first = d;
                last = d;
            }
        }
        
        uint32_t number = (first-'0')*10 + (last-'0');
        result += number;
    }

    return result;
}

uint32_t getGold(std::vector<std::string>& lines) {
    const std::string lookAtTableNumbs[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9"};
    const std::string lookAtTableString[] = {"one", "two", "three", "four", "five", "six", "seven", "eight", "nine"};
    
    uint32_t gold = 0;
    for(std::string& line: lines) {
        char firstChar = '0';
        size_t firsPosition = std::string::npos;
        char lastChar = '0';
        size_t lastPosition = 0;

        for(size_t i = 0; i < line.length(); i++) {
            if(std::isdigit(line[i])) {
                if(firsPosition >= i) {
                    firsPosition = i;
                    firstChar = line[i];
                }
                if(lastPosition <= i) {
                    lastPosition = i;
                    lastChar = line[i];
                }
            }
        }
        // First occurrence of our written numbers
        for(uint32_t i = 0; i < 9; i++) {
            size_t pos = line.find(lookAtTableString[i]);
            if(pos != std::string::npos) {
                if(firsPosition >= pos) {
                    char c = (char)(i+1)+'0';
                    firsPosition = pos;
                    firstChar = c;
                }
            }
        }
        // Final occurrence
        for(uint32_t i = 0; i < 9; i++) {
            size_t pos = line.rfind(lookAtTableString[i]);
            if(pos != std::string::npos) {
                if(lastPosition <= pos) {
                    char c = (char)(i+1)+'0';
                    lastPosition = pos;
                    lastChar = c;
                }
            }
        }

        uint32_t num = (firstChar-'0')*10 + (lastChar-'0');
        gold += num;
    }


    return gold;
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

    std::vector<std::string> lines;

    // Parse
    std::string line;
    while(std::getline(inputFile, line)) {
        lines.push_back(line);
    }

    std::cout << "Silver: " << getSilver(lines) << "\n";
    std::cout << "Gold: " << getGold(lines) << "\n";
    return 0;
}