#include "pch.h"
#include "Loaders.hpp"
#include <zlib.h>
#include "DevDef.h"

void RawFileLoader::dump(RawFile* rawfile) {
    if (!rawfile || !rawfile->name || !rawfile->buffer || rawfile->compressedLen <= 0) {
        return;
    }
    std::filesystem::path filePath = std::filesystem::path("S2MP-Mod/dump") / rawfile->name;

    if (std::filesystem::exists(filePath)) {
        return;
    }
    std::filesystem::create_directories(filePath.parent_path());

    bool decompressedOK = false;
    std::vector<unsigned char> decompressed;
    size_t decompressedSize = 0;

    if (rawfile->len > 0) {
        decompressed.resize(rawfile->len);

        z_stream strm{};
        strm.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(rawfile->buffer));
        strm.avail_in = static_cast<uInt>(rawfile->compressedLen);
        strm.next_out = decompressed.data();
        strm.avail_out = static_cast<uInt>(decompressed.size());

        int ret = inflateInit(&strm);
        if (ret == Z_OK) {
            ret = inflate(&strm, Z_FINISH);
            if (ret == Z_STREAM_END || ret == Z_OK) {
                decompressedSize = strm.total_out;
                decompressed.resize(decompressedSize);
                decompressedOK = true;
            }
            inflateEnd(&strm);
        }
    }

    std::ofstream out(filePath, std::ios::binary | std::ios::out | std::ios::trunc);
    if (!out.is_open()) {
        Console::printf("Failed to open file '%s' for writing", filePath.string().c_str());
        return;
    }

    if (decompressedOK) {
        out.write(reinterpret_cast<const char*>(decompressed.data()), decompressed.size());

#ifdef DEVELOPMENT_BUILD
        Console::printf("Dumped RawFile: '%s' (%zu bytes decompressed from %d bytes)", rawfile->name, decompressed.size(), rawfile->compressedLen);
#else 
        Console::printf("Dumped RawFile: '%s'", rawfile->name);
#endif
    }
    else {
        out.write(rawfile->buffer, rawfile->len);
        Console::printf("Dumped (raw): '%s' (%d bytes)", rawfile->name, rawfile->len);
    }

    out.close();
}

void RawFileLoader::loadCustom(RawFile* rawfile) {
    if (!rawfile || !rawfile->name)
        return;

    std::filesystem::path filePath = std::filesystem::path("S2MP-Mod") / rawfile->name;

    if (!std::filesystem::exists(filePath))
        return; // No custom file, skip

    Console::printf("Loading custom raw file from '%s'", filePath.string().c_str());

}