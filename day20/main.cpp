#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <queue>
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

enum class ModuleType {
    FlipFlop, Conjunction, Broadcast, Empty
};

enum class PulseType {LOW, HIGH};

struct Module {
    // Common
    string name;
    ModuleType type;
    vector<string> destinationModules;

    // Data
    PulseType power;
};

struct Message {
    string from;
    string to;
    PulseType power;
};

int64_t getSilver(unordered_map<string, Module>& modules, unordered_map<string, unordered_map<string, PulseType>> conjunctionMemory) {

    uint32_t lowPulsesSent = 0;
    uint32_t highPulsesSent = 0;

    cout << "Running " << "\n\n";

    for(uint32_t i = 0; i < 1000; i++) {

        queue<Message> messages;
        messages.push({"button", "broadcaster", PulseType::LOW});
        lowPulsesSent++;

        while(!messages.empty()) {
            Message messageSender = messages.front();
            messages.pop();
            
            Module modFrom = modules.at(messageSender.from);

            if(modules.find(messageSender.to) == modules.end()) {
                cout << "Module " << messageSender.to << " Not found\n";
                continue;
            }
            Module& modTo = modules.at(messageSender.to);

            cout << modFrom.name;
            if(messageSender.power == PulseType::LOW) cout << " -low-> ";
            if(messageSender.power == PulseType::HIGH) cout << " -high-> ";
            cout << modTo.name;
            cout << "\n";
            
            // Receiver logic
            switch(modTo.type) {
                case ModuleType::Empty:
                    // do nothing
                    break;
                case ModuleType::Broadcast:
                    // send the same pulse
                    for(const string& destination: modTo.destinationModules) {
                        messages.push({modTo.name, destination, messageSender.power});
                        lowPulsesSent++;
                    }
                    break;
                case ModuleType::FlipFlop:
                    // Flip on low pulse
                    if(messageSender.power == PulseType::LOW) {
                        if(modTo.power == PulseType::LOW) {
                            // If it's on send a HIGH pulse and turn off
                            modTo.power = PulseType::HIGH;
                            for(const string& d: modTo.destinationModules) {
                                messages.push({modTo.name, d, PulseType::HIGH});
                                highPulsesSent++;
                            }
                        }else{
                            // If it's off send a LOW pulse and turn on
                            modTo.power = PulseType::LOW;
                            for(const string& d: modTo.destinationModules) {
                                messages.push({modTo.name, d, PulseType::LOW});
                                lowPulsesSent++;
                            }
                        }
                    } // Do nothing on high pulses
                    break;
                case ModuleType::Conjunction:
                    // Update memory
                    conjunctionMemory.at(modTo.name)[modFrom.name] = messageSender.power;

                    bool allHigh = true;
                    unordered_map<string, PulseType> memoryConjunction = conjunctionMemory.at(modTo.name);
                    for(const auto& input: memoryConjunction) {
                        if(input.second == PulseType::LOW) {
                            allHigh = false;
                        }
                    }

                    for(const auto& d: modTo.destinationModules) {
                        if(allHigh) {
                            messages.push({modTo.name, d, PulseType::LOW});
                            lowPulsesSent++;
                        }else{
                            messages.push({modTo.name, d, PulseType::HIGH});
                            highPulsesSent++;
                        }
                    }
                    break;
            }
        }

    }

    cout << "Low pulses sent: " << lowPulsesSent << ", high pulses sent: " << highPulsesSent << "\n";

    return lowPulsesSent*highPulsesSent;
}

uint64_t getGold(unordered_map<string, Module>& modules, unordered_map<string, unordered_map<string, PulseType>> conjunctionMemory, const unordered_map<string, unordered_map<string, PulseType>>& conjunctionMemoryUntouched) {

    string modulePointingToRx;
    for(const auto& mm: modules) {
        for(const string& d: mm.second.destinationModules) {
            if(d == "rx")modulePointingToRx = mm.second.name;
        }
    }

    vector<pair<Module, int64_t>> modulesPointingToLast;
    for(const auto& mm: modules) {
        for(const string& d: mm.second.destinationModules) {
            if(d == modulePointingToRx)modulesPointingToLast.push_back({mm.second, -1});
        }
    }

    cout << "last module (pointing to RX) is: " << modulePointingToRx << "\n";
    for(const auto& conn: modulesPointingToLast) {
        cout << "Module before: " << conn.first.name << "\n";
    }


    cout << "Running " << "\n\n";
    cout << "Le number I hope: " << 3889L*3911L*3947L*4013L << "\n";

    bool waitingForLowRX = true;
    uint64_t cycles = 0;
    while(waitingForLowRX && cycles < 100000000) {

        queue<Message> messages;
        messages.push({"button", "broadcaster", PulseType::LOW});

        

        while(!messages.empty()) {
            Message messageSender = messages.front();
            messages.pop();
            
            Module modFrom = modules.at(messageSender.from);


            if(modules.find(messageSender.to) == modules.end()) {
                //cout << "Module " << messageSender.to << " Not found\n";
                if(messageSender.to == "rx" && messageSender.power == PulseType::LOW) {
                    if(messageSender.power == PulseType::LOW)cout << "IT IS LOW" << "\n";
                    cout << "We got it at: " << cycles << "\n";
                    waitingForLowRX = false;
                }
                
                continue;
            }
            Module& modTo = modules.at(messageSender.to);
        
            // Receiver logic
            switch(modTo.type) {
                case ModuleType::Empty:
                    // do nothing
                    break;
                case ModuleType::Broadcast:
                    // send the same pulse
                    for(const string& destination: modTo.destinationModules) {
                        messages.push({modTo.name, destination, messageSender.power});
                    }
                    break;
                case ModuleType::FlipFlop:
                    // Flip on low pulse
                    if(messageSender.power == PulseType::LOW) {
                        if(modTo.power == PulseType::LOW) {
                            // If it's on send a HIGH pulse and turn off
                            modTo.power = PulseType::HIGH;
                            for(const string& d: modTo.destinationModules) {
                                messages.push({modTo.name, d, PulseType::HIGH});
                            }
                        }else{
                            // If it's off send a LOW pulse and turn on
                            modTo.power = PulseType::LOW;
                            for(const string& d: modTo.destinationModules) {
                                messages.push({modTo.name, d, PulseType::LOW});
                            }
                        }
                    } // Do nothing on high pulses
                    break;
                case ModuleType::Conjunction:
                    // Update memory
                    conjunctionMemory.at(modTo.name)[modFrom.name] = messageSender.power;

                    bool allHigh = true;
                    bool zk = false;
                    bool qs = false;
                    bool jf = false;
                    bool ks = false;
                    unordered_map<string, PulseType> memoryConjunction = conjunctionMemory.at(modTo.name);
                    for(const auto& input: memoryConjunction) {
                        if(modTo.name == "hj") {
                            if(input.second == PulseType::HIGH){
                                if(input.first == "zk")zk = true;
                                if(input.first == "qs")qs = true;
                                if(input.first == "jf")jf = true;
                                if(input.first == "ks")ks = true;
                            }

                            /*
                            if(input.second == PulseType::HIGH){
                                cout << "From: " << input.first << " memory high on: " << cycles << "\n";
                            }
                            */
                        }

                        
                        if(input.second == PulseType::LOW) {
                            allHigh = false;
                        }
                    }
                    
                    if(zk && qs) {
                        cout << "ZK && QS are both high at " << cycles << "\n";
                    }

                    /*
                    if(allHigh) {
                        if(modTo.name == "zk")cout << "zk high on: " << cycles << "\n";
                        if(modTo.name == "ks")cout << "ks high on: " << cycles << "\n";
                        if(modTo.name == "qs")cout << "qs high on: " << cycles << "\n";
                        if(modTo.name == "jf")cout << "jf high on: " << cycles << "\n";
                    }
                    */
                    

                    for(const auto& d: modTo.destinationModules) {
                        if(allHigh) {
                            messages.push({modTo.name, d, PulseType::LOW});
                        }else{
                            messages.push({modTo.name, d, PulseType::HIGH});
                        }
                    }
                    break;
            }
        }

       

        cycles++;

        /*
        vector<pair<Module, int64_t>> modulesPointingToLast;
        for(auto& pairModPoints: modulesPointingToLast) {
            Module m = modules.at(pairModPoints.first.name);
            const unordered_map<string, PulseType> memoryConjunctionChanged = conjunctionMemory.at(m.name);
            const unordered_map<string, PulseType> memoryConjunctionUnchanged = conjunctionMemoryUntouched.at(m.name);

            for(const auto& memUnchanged: memoryConjunctionUnchanged) {
                if(memUnchanged.second != )
            }
            if(m.power != pairModPoints.first.power) {
                cout << "change of module " << m.name << " at " << cycles << "\n";
                pairModPoints.second = cycles;
            }
        }
        */

    }

    return cycles;
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

    unordered_map<string, Module> modules;
    unordered_map<string, unordered_map<string, PulseType>> conjunctionMemory;
    
    // Get width and height
    std::string line;
    while(std::getline(inputFile, line)) {
        string namePart = splitString(line, " ")[0];
        Module module;
        if(line[0] == '%') {
            module.name = namePart.substr(1, namePart.size()-1);
            module.type = ModuleType::FlipFlop;
            module.power = PulseType::LOW;
        }else if(line[0] == '&') {
            module.name = namePart.substr(1, namePart.size()-1);
            module.type = ModuleType::Conjunction;
        }else{
            module.name = namePart;
            module.type = ModuleType::Broadcast;
        }

        string destPart = splitString(line, " -> ")[1];
        vector<string> dest = splitString(destPart, ", ");
        for(const string& d: dest) {
            module.destinationModules.push_back(d);
        }

        modules.insert({module.name, module});
    }

    // Save all connections for the conjunctions
    for(const auto& mm: modules) {
        if(mm.second.type == ModuleType::Conjunction) {

            // Get all modules that are sending to it
            unordered_map<string, PulseType> memoryConjunction;
            for(const auto& possbleM: modules) {
                for(const string& destMod: possbleM.second.destinationModules) {
                    if(destMod == mm.second.name) {
                        memoryConjunction.insert({possbleM.second.name, PulseType::LOW});
                    }
                }
            }
            conjunctionMemory.insert({mm.second.name, memoryConjunction});

        }
    }

    unordered_map<string, unordered_map<string, PulseType>> conjunctionMemoryUntouched;

    for(const auto& mm: modules) {
        if(mm.second.type == ModuleType::Conjunction) {

            // Get all modules that are sending to it
            unordered_map<string, PulseType> memoryConjunction;
            for(const auto& possbleM: modules) {
                for(const string& destMod: possbleM.second.destinationModules) {
                    if(destMod == mm.second.name) {
                        memoryConjunction.insert({possbleM.second.name, PulseType::LOW});
                    }
                }
            }
            conjunctionMemory.insert({mm.second.name, memoryConjunction});
        }
    }

    // Add button && output
    vector<string> empty;
    modules.insert({"button", {"button", ModuleType::Empty, empty, PulseType::LOW}});
    modules.insert({"output", {"output", ModuleType::Empty, empty, PulseType::LOW}});
    
    // Debug
    for(const auto& mm: modules) {
        Module m = mm.second;
        switch(m.type) {
            case ModuleType::Broadcast:
                break;
            case ModuleType::FlipFlop:
                cout << "%";
                break;
            case ModuleType::Conjunction:
                cout << "&";
                break;
            case ModuleType::Empty:
                break;
        }
        cout << m.name << " -> ";
        for(size_t i = 0; i < m.destinationModules.size(); i++) {
            cout << m.destinationModules[i];
            if(i < m.destinationModules.size()-1)cout << ", ";
        }
        cout << "\n";
    }
    

    //cout << "Silver: " <<  getSilver(modules, conjunctionMemory) << "\n";
    cout << "Gold: " <<  getGold(modules, conjunctionMemory, conjunctionMemoryUntouched) << "\n";

    return 0;
}