#ifndef __medleap_Config__
#define __medleap_Config__

#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include "math/Vector3.h"

/** Stores/loads configuration values in a file. */
class Config
{
public:
    
    template <typename T>
    void putValue(const std::string& name, const T& value)
    {
        std::stringstream ss;
        ss << value;
        values[name] = ss.str();
    }
    
    template <typename T>
    T getValue(const std::string& paramName)
    {
        T result;
        std::unordered_map<std::string, std::string>::iterator it = values.find(paramName);
        if (it != values.end()) {
            std::stringstream ss(it->second);
            std::cout << it->second << std::endl;
            ss >> result;
            return result;
        }
        return result;
    }
    
    void clear();
    void load(const std::string& fileName);
    void save(const std::string& fileName);
    
private:
    std::unordered_map<std::string, std::string> values;
};

#endif // __medleap_Config__