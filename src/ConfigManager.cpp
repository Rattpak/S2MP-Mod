///////////////////////////////////////////////
//              Config Manager
//	Just for the ext con to save preferences
////////////////////////////////////////////////
#include "pch.h"
#include "ConfigManager.h"
#include <fstream>
#include <string>

bool ConfigManager::readConfigValue(const std::string& filename, const std::string& key) {
    std::ifstream configFile(filename);
    std::string line;
    while (getline(configFile, line)) {
        //assuming the line format is key=value
        size_t delimiterPos = line.find("=");
        std::string fileKey = line.substr(0, delimiterPos);
        std::string fileValue = line.substr(delimiterPos + 1);

        if (fileKey == key) {
            return fileValue == "true";
        }
    }
    return false;
}

void ConfigManager::writeConfigValue(const std::string& filename, const std::string& key, bool value) {
    std::ofstream configFile(filename);
    configFile << key << "=" << (value ? "true" : "false") << std::endl;
    configFile.close();
}

bool ConfigManager::configExists(const std::string& filename) {
    std::ifstream testFile(filename);
    if (testFile.good()) {
        return true;
    }
    else {
        return false;
    }
    testFile.close();
}