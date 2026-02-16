//taken from old t6sp-mod build. TODO: update this

#include "pch.h"
#include "Logfile.hpp"
#include "Console.hpp"
#include <fstream>
#include <filesystem>
#include "GameUtil.hpp"
#include "Hook.hpp"
#include "DevDef.h"

bool Logfile::enabled = false;

void Logfile::init() {
	//delete logfile if it exists
	std::string filePath = "main/s2mp_console.log";
	std::filesystem::path logFilePath(filePath);
	try {
		if (std::filesystem::exists(logFilePath)) {
			std::filesystem::remove(logFilePath);
		}
	}
	catch (const std::filesystem::filesystem_error& e) {
#ifdef DEVELOPMENT_BUILD
		Console::devPrint("Logfile init error");
#endif // DEVELOPMENT_BUILD

	}
}

void Logfile::setEnabled(bool b) {
	Logfile::enabled = b;
}

void Logfile::append(std::string& text) {
	if (Logfile::enabled) {
		std::string filePath = "main/s2mp_console.log";
		std::filesystem::path dirPath = std::filesystem::path(filePath).parent_path();
		if (!std::filesystem::exists(dirPath)) {
			std::filesystem::create_directories(dirPath);
		}
		std::ofstream outFile(filePath, std::ios::out | std::ios::app);
		if (!outFile) {
			return;
		}

		//check for newline at end
		if (!text.empty() && text.back() != '\n') {
			text += '\n';
		}
		outFile.write(text.c_str(), text.length());
		outFile.close();
	}
}