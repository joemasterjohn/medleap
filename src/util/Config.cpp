#include "Config.h"
#include <fstream>

using namespace std;

void Config::clear()
{
    values.clear();
}
void Config::load(const std::string& fileName)
{
    ifstream fs(fileName);
    string line;
    
    if (fs.is_open()) {
        while (getline(fs, line)) {
            int splitPos = line.find("=");
            string key = line.substr(0, splitPos);
            string value = line.substr(splitPos+1);
            values[key] = value;
        }
    }
    
    fs.close();
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