#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
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

struct Part {
    int64_t rating[4];
};

struct PartRange {
    pair<int64_t, int64_t> range[4];
};

int64_t getCombinations(const PartRange& partRange) {
    int64_t x = partRange.range[0].second - partRange.range[0].first +1;
    int64_t m = partRange.range[1].second - partRange.range[1].first +1;
    int64_t a = partRange.range[2].second - partRange.range[2].first +1;
    int64_t s = partRange.range[3].second - partRange.range[3].first +1;
    return x*m*a*s;
}

uint8_t getIndexVal(char val) {
    switch(val) {
        case 'x':
            return 0;
        case 'm':
            return 1;
        case 'a':
            return 2;
        case 's':
            return 3;
        default:
            return 0;
    }
}

enum class Operation {
    NONE, LESS_THAN, GREATER_THAN
};

bool checksOperation(const Part& part, char valToCalculate, Operation o, int64_t value) {
    switch(o) {
        case Operation::NONE:
            return true;
            break;
        case Operation::LESS_THAN:
            if(part.rating[getIndexVal(valToCalculate)] < value)return true;
            break;
        case Operation::GREATER_THAN:
            if(part.rating[getIndexVal(valToCalculate)] > value)return true;
            break;
    }
    return false;
}

struct Condition {
    char type;
    Operation op;
    int64_t value;
    string workflowIfChecks;
};

int64_t getSilver(const unordered_map<string,vector<Condition>>& workflows, const vector<Part>& parts) {
    int64_t silver = 0;
    
    for(const Part part: parts) {
        enum class Status {IN_PROGRESS, ACCEPTED, REJECTED};
        Status status = Status::IN_PROGRESS;
        string work = "in";

        while(status == Status::IN_PROGRESS) {
            vector<Condition> workflow = workflows.at(work);

            for(const Condition& workflow: workflow) {
                bool passes = checksOperation(part, workflow.type, workflow.op, workflow.value);
                if(passes) {
                    if(workflow.workflowIfChecks == "A") {
                        status = Status::ACCEPTED;
                    }else if(workflow.workflowIfChecks == "R"){
                        status = Status::REJECTED;
                    }else{
                        work = workflow.workflowIfChecks;
                    }
                    break;
                }
            }
        }

        if(status == Status::ACCEPTED) {
            silver += part.rating[0] + part.rating[1] + part.rating[2] + part.rating[3];
        }
    }


    return silver;
}

pair<pair<int64_t,int64_t>, pair<int64_t,int64_t>> splitRangeOnOperation(const pair<int64_t,int64_t>& range, const Operation op, int64_t value) {
    pair<int64_t,int64_t> ChecksCondition = {0,0};
    pair<int64_t,int64_t> notChecksCondition = {0,0};

    switch(op) {
        case Operation::LESS_THAN:
            if(range.second < value) { // Range pass
                ChecksCondition = range;
            }else{
                if(range.first >= value) { // Range not checks cond entirely
                    notChecksCondition = range;
                }else{ // construct new ranges
                    ChecksCondition = {range.first, value-1};
                    notChecksCondition = {value, range.second};
                }
            }
            break;
        case Operation::GREATER_THAN:
            if(range.first > value) { // every range pass
                ChecksCondition = range;
            }else{
                if(range.second <= value) { // Range not checks cond entirely
                    notChecksCondition = range;
                }else{ // construct new ranges
                    ChecksCondition = {value+1, range.second};
                    notChecksCondition = {range.first, value};
                }
            }
            break;
        case Operation::NONE:
            ChecksCondition = range;
            break;
    }

    return {ChecksCondition, notChecksCondition};
}

void putRanges(const unordered_map<string,vector<Condition>>& workflows, vector<PartRange>& acceptedRanges, const PartRange& partRange, const string& workflowName) {
    vector<Condition> workflow = workflows.at(workflowName);

    PartRange operateRange = partRange;
    for(const Condition& cond: workflow) {
        uint8_t indexVal = getIndexVal(cond.type);
        pair<pair<int64_t,int64_t>, pair<int64_t,int64_t>> pairRangesAcceptedRejected = splitRangeOnOperation(operateRange.range[indexVal], cond.op, cond.value);
        if(pairRangesAcceptedRejected.first.first != 0) {
            // Operate in the next workflow (except if it's R or A)
            // Get new accepted Range
            PartRange rangeToInsert = operateRange;
            rangeToInsert.range[indexVal] = pairRangesAcceptedRejected.first; // Get accepted range

            // Insert range
            if(cond.workflowIfChecks == "A") {
                acceptedRanges.push_back(rangeToInsert);
            }
            
            if(cond.workflowIfChecks != "A" && cond.workflowIfChecks != "R") {
                putRanges(workflows, acceptedRanges, rangeToInsert, cond.workflowIfChecks);
            }
        }
        if(pairRangesAcceptedRejected.second.first != 0) {
            operateRange.range[indexVal] = pairRangesAcceptedRejected.second; // continue with rejected
        }
    }
}

int64_t getGold(const unordered_map<string,vector<Condition>>& workflows) {
    int64_t gold = 0;
    
    string work = "in";

    vector<PartRange> acceptedRanges;

    PartRange partRange;
    partRange.range[0] = {1, 4000};
    partRange.range[1] = {1, 4000};
    partRange.range[2] = {1, 4000};
    partRange.range[3] = {1, 4000};

    putRanges(workflows, acceptedRanges, partRange, "in");

    for(uint32_t i = 0; i < acceptedRanges.size(); i++) {
        gold += getCombinations(acceptedRanges[i]);
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

    unordered_map<string,vector<Condition>> workflows;
    vector<Part> parts;

    // Get width and height
    std::string line;
    bool workflowParsing = true;
    while(std::getline(inputFile, line)) {
        if(line.empty()) {
            workflowParsing = false;
        }else{
            if(workflowParsing) {
                vector<string> st = splitString(line, "{");
                string workflowName = st[0];
                vector<string> listWorkflow = splitString(st[1].substr(0, st[1].size()-1), ",");
                vector<Condition> conditions;
                for(const string& s: listWorkflow) {
                    Condition cond;
                    if(s.find(":") != string::npos) {
                        // has operation
                        vector<string> splitPartOper = splitString(s, ":");
                        string partOper = splitPartOper[0];
                        string goToWorkflow = splitPartOper[1];
                        cond.workflowIfChecks = goToWorkflow;
                        if(partOper.find("<") != string::npos) {
                            vector<string> splitLess = splitString(partOper, "<");
                            cond.op = Operation::LESS_THAN;
                            cond.type = splitLess[0][0];
                            cond.value = atoi(splitLess[1].c_str());
                        }else{
                            vector<string> splitMayor = splitString(partOper, ">");
                            cond.op = Operation::GREATER_THAN;
                            cond.type = splitMayor[0][0];
                            cond.value = atoi(splitMayor[1].c_str());
                        }
                    }else{
                        // maps directly to another workflow
                        cond.op = Operation::NONE;
                        cond.workflowIfChecks = s;
                    }
                    conditions.push_back(cond);
                }

                workflows.insert({workflowName, conditions});
            }else{
                // xmas parts
                vector<string> xmaxParts = splitString(line.substr(1, line.size()-2), ",");
                Part p = {
                    atoi(splitString(xmaxParts[0], "=")[1].c_str()),
                    atoi(splitString(xmaxParts[1], "=")[1].c_str()),
                    atoi(splitString(xmaxParts[2], "=")[1].c_str()),
                    atoi(splitString(xmaxParts[3], "=")[1].c_str())
                };
                parts.push_back(p);
            }
        }
       
    }
    
    // Debug
    /*
    for(const auto& w: workflows) {
        cout << "w: " << w.first << " = {";
        for(const Condition& c: w.second) {
            if(c.op != Operation::NONE) {
                cout << c.type;
                if(c.op == Operation::LESS_THAN) cout << "<";
                if(c.op == Operation::GREATER_THAN) cout << ">";
                cout << c.value;
            }else{
                cout << c.workflowIfChecks;
            }
            cout << ",";
        }
        cout << "}\n";
    }
    */

    cout << "Silver: " << getSilver(workflows, parts) <<  "\n";
    cout << "Gold: " <<  getGold(workflows) << "\n";

    return 0;
}