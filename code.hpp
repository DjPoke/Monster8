#ifndef APPLY_CODE_HPP
#define APPLY_CODE_HPP

#include <cstdint>
#include <functional>
#include <string>
#include <iostream>
#include <cstring>
#include <iomanip>
#include <vector>

#include "functions.hpp"

class ApplyCode {
public:
    ApplyCode();

    // variables and arrays
    uint8_t memory[0x5000000];
    std::string label[0x1000000][2];

    uint32_t spritesheetCount;
    std::vector<std::string> spritesheetPath;
    std::vector<uint32_t> spritesheetAddress;
    std::vector<uint32_t> spritesheetLength;
    std::vector<uint32_t> spritesheetWidth;
    std::vector<uint32_t> spritesheetHeight;

    std::vector<uint32_t> spriteCount;
    std::vector<std::vector<uint32_t>> spriteAddress;
    std::vector<std::vector<uint32_t>> spriteLength;
    std::vector<std::vector<uint32_t>> spriteWidth;
    std::vector<std::vector<uint32_t>> spriteHeight;

    uint32_t tilesetCount;
    std::vector<std::string> tilesetPath;
    std::vector<uint32_t> tilesetAddress;
    std::vector<uint32_t> tilesetLength;
    std::vector<uint32_t> tilesetWidth;
    std::vector<uint32_t> tilesetHeight;

    std::vector<uint32_t> tileCount;
    std::vector<std::vector<uint32_t>> tileAddress;
    std::vector<std::vector<uint32_t>> tileLength;
    std::vector<std::vector<uint32_t>> tileWidth;
    std::vector<std::vector<uint32_t>> tileHeight;

    uint32_t soundCount;
    std::vector<std::string> soundPath;
    std::vector<uint32_t> soundAddress;
    std::vector<uint32_t> soundLength;

    uint32_t musicCount;
    std::vector<std::string> musicPath;
    std::vector<uint32_t> musicAddress;
    std::vector<uint32_t> musicLength;

    uint32_t pass;
    uint32_t lastLabelLine;

    uint32_t PC;

    std::string asmFile;
    std::string binFile;
    std::string emuFile;

    bool ToMemory(uint32_t a, uint8_t d); // write byte to memory
    bool CheckGarbages(std::string s); // check for garbages in string
    uint32_t FindLabelAddress(std::string s, uint32_t l); // find address of label
    void WriteLabelOrValue(uint32_t r, std::string dat, uint32_t l); // write label or value to memory
    std::string CompileLine(std::string s, uint32_t l); // compile line of assembly code
    bool StringToInt(const std::string &str, int &outValue);

    void setStatusBarUpdater(const std::function<void(const std::string&)> &updater);
    void updateStatus(const std::string& msg);

private:
    std::function<void(const std::string&)> statusBarUpdater;
};

#endif // APPLY_CODE_HPP