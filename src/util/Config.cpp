#include "Config.h"
#include <fstream>

using namespace std;

Config::~Config()
{
}

void Config::clear()
{
    values.clear();
}

bool Config::load(const std::string& fileName)
{
    ifstream fs(fileName);
    string line;
    bool foundFile = false;
    
    if (fs.is_open()) {
        foundFile = true;
        while (getline(fs, line)) {
            int splitPos = line.find("=");
            string key = line.substr(0, splitPos);
            string value = line.substr(splitPos+1);
            values[key] = value;
        }
    }
    
    fs.close();
    return foundFile;
}

void Config::save(const std::string& fileName)
{
    ofstream fs(fileName);
    unordered_map<string, string>::iterator it = values.begin();
    
    while (it != values.end()) {
        fs << it->first << "=" << it->second << endl;
        it++;
    }
    
    fs.close();
}