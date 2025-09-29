#pragma once
#include <unordered_map>
#include <string>
#include "structs.h"
#include <vector>

class DvarInterface {
private:
    //static std::unordered_map<std::string, std::string> userToEngineMap;
    static std::unordered_map<std::string, std::string> engineToUserMap;
    static std::unordered_map<std::string, std::string> descriptionMap;
    static void addDvarsWithName(const char* name);
    static void addAllMappings();
    static void addMapping(const std::string& userString, const std::string& engineString);
    static void addMapping(const std::string& userString, const std::string& engineString, const std::string& description);

    

public:
    static std::unordered_map<std::string, std::string> userToEngineMap;

    static void init();
    static bool setDvar(std::string& dvarname, std::vector<std::string> cmd);
    static std::string getDvarDescription(const std::string& engineStr);
    static unsigned int getDvarListSize();
    static std::string toEngineString(const std::string& userString);
    static std::string toUserString(const std::string& engineString);
    
};