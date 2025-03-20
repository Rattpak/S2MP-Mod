#pragma once
#include <unordered_map>
#include <string>
#include "structs.h"
#include <vector>

class DvarInterface {
private:
    static std::unordered_map<std::string, std::string> userToEngineMap;
    static std::unordered_map<std::string, std::string> engineToUserMap;

    static void addAllMappings();
    static void addMapping(const std::string& userString, const std::string& engineString);

public:
    static void init();
    static bool setDvar(std::string& dvarname, std::vector<std::string> cmd);
   
    static std::string toEngineString(const std::string& userString);
    static std::string toUserString(const std::string& engineString);
};