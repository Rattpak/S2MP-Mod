#pragma once
#include <string>
class ConfigManager {
public:
	static bool readConfigValue(const std::string& filename, const std::string& key);
	static void writeConfigValue(const std::string& filename, const std::string& key, bool value);
	static bool configExists(const std::string& filename);
};