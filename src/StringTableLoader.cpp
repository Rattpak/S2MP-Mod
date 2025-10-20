#include "pch.h"
#include "Loaders.hpp"

void StringTableLoader::dump(StringTable* table) {
    if (!table || !table->values || table->rowCount <= 0 || table->columnCount <= 0) {
        return;
    }

    std::filesystem::path filePath = std::filesystem::path("S2MP-Mod/dump") / table->name;
    if (std::filesystem::exists(filePath)) {
        return;
    }

    std::filesystem::create_directories(filePath.parent_path());
    std::ofstream out(filePath, std::ios::out | std::ios::trunc);
    if (!out.is_open()) {
        Console::printf("Failed to open file '%s' for writing\n", filePath.string().c_str());
        return;
    }

    //helper for csv cells
    auto writeCsvCell = [&](const char* str) {
        if (!str) {
            str = "";
        }

        std::string s(str);
        bool needsQuotes = s.find(',') != std::string::npos || s.find('"') != std::string::npos || s.find('\n') != std::string::npos || s.find('\r') != std::string::npos;

        if (needsQuotes) {
            out.put('"');
            for (char ch : s) {
                if (ch == '"') {
                    out.put('"'); //escape quotes
                }
                out.put(ch);
            }
            out.put('"');
        }
        else {
            out << s;
        }
    };

    //write csv
    for (int r = 0; r < table->rowCount; ++r) {
        for (int c = 0; c < table->columnCount; ++c) {
            const StringTableCell& cell = table->values[r * table->columnCount + c];
            writeCsvCell(cell.string);

            if (c < table->columnCount - 1) {
                out.put(',');
            }
        }
        out.put('\n');
    }

    out.close();
    Console::printf("Dumped: '%s'", filePath.string().c_str());
}

void StringTableLoader::loadCustom(StringTable* table) {
    if (!table || !table->name || !*table->name)
        return;

    std::filesystem::path filePath = std::filesystem::path("S2MP-Mod") / table->name;

    if (!std::filesystem::exists(filePath))
        return; //no custom

    Console::printf("Loading custom StringTable from '%s'", filePath.string().c_str());

    std::ifstream in(filePath);
    if (!in.is_open()) {
        Console::printf("Failed to open custom table: %s", filePath.string().c_str());
        return;
    }
    
    //TODO: finish
}