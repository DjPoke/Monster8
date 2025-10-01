#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include "M8.hpp"

Monster8::Monster8() {
    initialize();
}

void Monster8::initialize() {
    PC = 0x000400;
    opcode = 0x00;
    SP = 0x003FFF;

    memory.fill(0);
    screen.fill(0);
    sprites.fill(0);
    tilesets.fill(0);
    audio1.fill(0);
    audio2.fill(0);
    
    joystick = 0;

    stack.fill(0);
    A.fill(0x000000);
    D.fill(0x00);

    delay_timer = 0;
    sound_timer = 0;

    cursorX = 1;
    cursorY = 1;

    border = 107; // bordure violette
    paper = 107;   // fond violet
    pen = 0;       // texte noir
    drawFlag = false;

    for (int i = 0; i < 128; ++i) {
        for (int j = 0; j < 8; ++j) {
            memory[i * 8 + j] = fontset[i][j];
        }
    }

    // remplir la palette 0
    uint16_t cpt = 64000;

    for(int b = 0; b < 8; b++) {
        for(int g = 0; g < 4; g++) {
            for(int r = 0; r < 8; r++) {
                screen[cpt] = (r * 0x24);
                screen[cpt + 1] = (g * 0x55);
                screen[cpt + 2] = (b * 0x24);
                cpt += 3;
            }
        }
    }

    // remplir la palette 1 (monochrome)
    cpt = 64768;

    for(int g = 0; g < 256; g++) {
        screen[cpt] = g;
        screen[cpt + 1] = g;
        screen[cpt + 2] = g;
        cpt += 3;
    }
}


void Monster8::loadROM(const char* filename) {
    ifstream file(filename, ios::binary | ios::ate);
    if (!file.is_open()) {
        cerr << "Error: Unable to open ROM file." << endl;
        return;
    }

    streamsize size = file.tellg();
    file.seekg(0, ios::beg);

    vector<char> buffer(size);

    if(file.read(buffer.data(), size)) {
        for(size_t i = 0; i < buffer.size(); i++) {
            if ((0x0400 + i) < 0xFFFFFF) {
                memory[0x0400 + i] = buffer[i];
            } else if ((0x0400 + i) < 0x01FFFFFF) {
                sprites[(0x0400 + i) - 0x01000000] = buffer[i];
            } else if ((0x0400 + i) < 0x02FFFFFF) {
                tilesets[(0x0400 + i) - 0x02000000] = buffer[i];
            } else if ((0x0400 + i) < 0x03FFFFFF) {
                audio1[(0x0400 + i) - 0x03000000] = buffer[i];
            } else if ((0x0400 + i) < 0x04FFFFFF) {
                audio2[(0x0400 + i) - 0x04000000] = buffer[i];
            }
        }
    }

    file.close();
}

uint8_t Monster8::getOpcode() {
    uint8_t ret_value = memory[PC];
    PC = (PC + 1) % 0x1000000;

    return ret_value;
}

void Monster8::emulateCycle() {
    opcode = getOpcode();

    // loop halted til escape key pressed
    if(halted) {
        opcode = 0x01;
    }

    switch(opcode) {
        case 0x00: // NOP (1 opcode)
            break;
        case 0x01: // Halt (1 opcode)
            cout << "HALTED!" << endl;
            halted = true;
            break;
        case 0x02: // Jump to Direct Address(3 opcodes)
            {
                uint8_t pcValueHIGH = getOpcode();
                uint8_t pcValueMID = getOpcode();
                uint8_t pcValueLOW = getOpcode();
                PC = (pcValueHIGH << 16) | (pcValueMID << 8) | pcValueLOW;
            }
            break;
        case 0x03: // Call Subroutine (3 opcodes)
            {
                uint8_t subValueHIGH = getOpcode();
                uint8_t subValueMID = getOpcode();
                uint8_t subValueLOW = getOpcode();
                stack[SP] = PC;
                SP--;
                PC = (subValueHIGH << 16) | (subValueMID << 8) | subValueLOW;
            }
            break;
        case 0x04: // Return From Subroutine (1 opcode)
            SP++;
            PC = stack[SP];
            break;
        case 0x05: // Jump To Address In A0 (1 opcode)
            PC = A[0];
            break;
        case 0x06: // Jump To Address In A1 (1 opcode)
            PC = A[1];
            break;
        case 0x07: // Jump To Address In A2 (1 opcode)
            PC = A[2];
            break;
        case 0x08: // Jump To Address In A3 (1 opcode)
            PC = A[3];
            break;
        case 0x09: // Jump To Address In A4 (1 opcode)
            PC = A[4];
            break;
        case 0x0A: // Jump To Address In A5 (1 opcode)
            PC = A[5];
            break;
        case 0x0B: // Jump To Address In A6 (1 opcode)
            PC = A[6];
            break;
        case 0x0C: // Jump To Address In A7 (1 opcode)
            PC = A[7];
            break;
        case 0x0D: // jump if S flag is true
            {
                uint32_t addr = (getOpcode() << 16) | (getOpcode() << 8) | getOpcode();

                if(D[7] & (1 << FlagS)) {
                    PC = addr;
                }
            }
            break;
        case 0x0E:  // jump if S flag is false
            {
                uint32_t addr = (getOpcode() << 16) | (getOpcode() << 8) | getOpcode();

                if(!(D[7] & (1 << FlagS))) {
                    PC = addr;
                }
            }
            break;
        case 0x0F: // Jump if Zero flag is true (3 opcode)
            {
                uint32_t addr = (getOpcode() << 16) | (getOpcode() << 8) | getOpcode();

                if(D[7] & (1 << FlagZ)) {
                    PC = addr;
                }
            }
            break;
        case 0x10: // Jump if not Zero flag is true (3 opcode)
            {
                uint32_t addr = (getOpcode() << 16) | (getOpcode() << 8) | getOpcode();

                if(!(D[7] & (1 << FlagZ))) {
                    PC = addr;
                }
            }
            break;
        case 0x11: // Jump if Half-Carry is true
            {
                uint32_t addr = (getOpcode() << 16) | (getOpcode() << 8) | getOpcode();

                if(D[7] & (1 << FlagH)) {
                    PC = addr;
                }
            }
            break;
        case 0x12: // Jump if Half-Carry is false
            {
                uint32_t addr = (getOpcode() << 16) | (getOpcode() << 8) | getOpcode();

                if(!(D[7] & (1 << FlagH))) {
                    PC = addr;
                }
            }
            break;
        case 0x13: // jump is PV flag is true
            {
                uint32_t addr = (getOpcode() << 16) | (getOpcode() << 8) | getOpcode();

                if(D[7] & (1 << FlagPV)) {
                    PC = addr;
                }
            }
            break;
        case 0x14:  // jump is PV flag is false
            {
                uint32_t addr = (getOpcode() << 16) | (getOpcode() << 8) | getOpcode();

                if(!(D[7] & (1 << FlagPV))) {
                    PC = addr;
                }
            }
            break;
        case 0x15: // Jump if Carry flag is true (3 opcode)
            {
                uint32_t addr = (getOpcode() << 16) | (getOpcode() << 8) | getOpcode();

                if(D[7] & (1 << FlagC)) {
                    PC = addr;
                }
            }
            break;
        case 0x16: // Jump if Carry flag is false (3 opcode)
            {
                uint32_t addr = (getOpcode() << 16) | (getOpcode() << 8) | getOpcode();

                if(!(D[7] & (1 << FlagC))) {
                    PC = addr;
                }
            }
            break;
        case 0x17: // Return if S flag is true (1 opcode)
            if(D[7] & (1 << FlagS)) {
                SP++;
                PC = stack[SP];
            }
            break;
        case 0x18: // Return if S flag is false (1 opcode)
            if(!(D[7] & (1 << FlagS))) {
                SP++;
                PC = stack[SP];
            }
            break;
        case 0X19: // Return if Zero flag ss true
            if(D[7] & (1 << FlagZ)) {
                SP++;
                PC = stack[SP];
            }
            break;
        case 0x1A: // Return if Zero flag is false
            if(!(D[7] & (1 << FlagZ))) {
                SP++;
                PC = stack[SP];
            }
            break;
        case 0X1B: // Return if Half-Carry flag is true
            if(D[7] & (1 << FlagH)) {
                SP++;
                PC = stack[SP];
            }
            break;
        case 0X1C: // Return if Half-Carry flag is false
            if(!(D[7] & (1 << FlagH))) {
                SP++;
                PC = stack[SP];
            }
            break;
        case 0X1D: // Return if overflow flag is true
            if(D[7] & (1 << FlagPV)) {
                SP++;
                PC = stack[SP];
            }
            break;
        case 0X1E: // Return if overflow flag is false
            if(!(D[7] & (1 << FlagPV))) {
                SP++;
                PC = stack[SP];

            }
            break;
        case 0X1F: // Return if Carry flag is true
            if(D[7] & (1 << FlagC)) {
                SP++;
                PC = stack[SP];
            }
            break;
        case 0x20: // Return if Carry flag is false
            if(!(D[7] & (1 << FlagC))) {
                SP++;
                PC = stack[SP];
            }
            break;
        case 0x21: // Set Zero Flag (1 opcode)
            D[7] |= (1 << FlagZ);
            break;
        case 0x22: // Reset Zero Flag (1 opcode)
            D[7] &= ~(1 << FlagZ);
            break;
        case 0x23: // Set Carry Flag (1 opcode)
            D[7] |= (1 << FlagC);
            break;
        case 0x24: // Reset Carry Flag (1 opcode)
            D[7] &= ~(1 << FlagC);
            break;
        case 0x25: // Compare D0 with memory at A0 (1 opcode)
            {
                // set or reset flags
                D[7] = GetFlagsValue(D[0], memory[A[0]], "CP8");
            }
            break;
        case 0x26: // Compare D0 with D1 (1 opcode)
            {
                // set or reset flags
                D[7] = GetFlagsValue(D[0], D[1], "CP8");
            }
            break;
        case 0x27: // Compare D0 with Immediate Value (2 opcode)
            {
                // set or reset flags
                D[7] = GetFlagsValue(D[0], getOpcode(), "CP8");
            }
            break;
        case 0x28: // Load Value Into 24 bits Register A0 (3 opcodes)
            A[0] = (getOpcode() << 16) | (getOpcode() << 8) | getOpcode();
            break;
        case 0x29: // Load Value Into 24 bits Register A1 (3 opcodes)
            A[1] = (getOpcode() << 16) | (getOpcode() << 8) | getOpcode();
            break;
        case 0x2A: // Load Value Into 24 bits Register A2 (3 opcodes)
            A[2] = (getOpcode() << 16) | (getOpcode() << 8) | getOpcode();
            break;
        case 0x2B: // Load Value Into 24 bits Register A3 (3 opcodes)
            A[3] = (getOpcode() << 16) | (getOpcode() << 8) | getOpcode();
            break;
        case 0x2C: // Load Value Into 24 bits Register A4 (3 opcodes)
            A[4] = (getOpcode() << 16) | (getOpcode() << 8) | getOpcode();
            break;
        case 0x2D: // Load Value Into 24 bits Register A5 (3 opcodes)
            A[5] = (getOpcode() << 16) | (getOpcode() << 8) | getOpcode();
            break;
        case 0x2E: // Load Value Into 24 bits Register A6 (3 opcodes)
            A[6] = (getOpcode() << 16) | (getOpcode() << 8) | getOpcode();
            break;
        case 0x2F: // Load Value Into 24 bits Register A7 (3 opcodes)
            A[7] = (getOpcode() << 16) | (getOpcode() << 8) | getOpcode();
            break;
        case 0x30: // Load Value Into 8 bits Register D0 (2 opcodes)
            D[0] = getOpcode();
            break;
        case 0x31: // Load Value Into 8 bits Register D1 (2 opcodes)
            D[1] = getOpcode();
            break;
        case 0x32: // Load Value Into 8 bits Register D2 (2 opcodes)
            D[2] = getOpcode();
            break;
        case 0x33: // Load Value Into 8 bits Register D3 (2 opcodes)
            D[3] = getOpcode();
            break;
        case 0x34: // Load Value Into 8 bits Register D4 (2 opcodes)
            D[4] = getOpcode();
            break;
        case 0x35: // Load Value Into 8 bits Register D5 (2 opcodes)
            D[5] = getOpcode();
            break;
        case 0x36: // Load Value Into 8 bits Register D6 (2 opcodes)
            D[6] = getOpcode();
            break;
        case 0x37: // Load [A0] into D0 (1 opcode)
            D[0] = memory[A[0]];
            break;
        case 0x38: // Load [A1] into D1 (1 opcode)
            D[1] = memory[A[1]];
            break;
        case 0x39: // Load [A2] into D2 (1 opcode)
            D[2] = memory[A[2]];
            break;
        case 0x3A: // Load [A3] into D3 (1 opcode)
            D[3] = memory[A[3]];
            break;
        case 0x3B: // Load [A4] into D4 (1 opcode)
            D[4] = memory[A[4]];
            break;
        case 0x3C: // Load [A5] into D5 (1 opcode)
            D[5] = memory[A[5]];
            break;
        case 0x3D: // Load [A6] into D6 (1 opcode)
            D[6] = memory[A[6]];
            break;
        case 0x3E: // Load D0 into [A0] (1 opcode)
            memory[A[0]] = D[0];
            break;
        case 0x3F: // Load D1 into [A1] (1 opcode)
            memory[A[1]] = D[1];
            break;
        case 0x40: // Load D2 into [A2] (1 opcode)
            memory[A[2]] = D[2];
            break;
        case 0x41: // Load D3 into [A3] (1 opcode)
            memory[A[3]] = D[3];
            break;
        case 0x42: // Load D4 into [A4] (1 opcode)
            memory[A[4]] = D[4];
            break;
        case 0x43: // Load D5 into [A5] (1 opcode)
            memory[A[5]] = D[5];
            break;
        case 0x44: // Load D6 into [A6] (1 opcode)
            memory[A[6]] = D[6];
            break;
        case 0X45: // Copy D0 into D1 (1 opcode)
            D[1] = D[0];
            break;
        case 0X46: // Copy D0 into D2 (1 opcode)
            D[2] = D[0];
            break;
        case 0x47: // Copy D0 into D3 (1 opcode)
            D[3] = D[0];
            break;
        case 0x48: // Copy D0 into D4 (1 opcode)
            D[4] = D[0];
            break;
        case 0x49: // Copy D0 into D5 (1 opcode)
            D[5] = D[0];
            break;
        case 0x4A: // Copy D0 into D6 (1 opcode)
            D[6] = D[0];
            break;
        case 0X4B: // Copy D1 into D0 (1 opcode)
            D[0] = D[1];
            break;
        case 0X4C: // Copy D1 into D2 (1 opcode)
            D[2] = D[1];
            break;
        case 0x4D: // Copy D1 into D3 (1 opcode)
            D[3] = D[1];
            break;
        case 0x4E: // Copy D1 into D4 (1 opcode)
            D[4] = D[1];
            break;
        case 0x4F: // Copy D1 into D5 (1 opcode)
            D[5] = D[1];
            break;
        case 0x50: // Copy D1 into D6 (1 opcode)
            D[6] = D[1];
            break;
        case 0X51: // Copy D2 into D0 (1 opcode)
            D[0] = D[2];
            break;
        case 0X52: // Copy D2 into D1 (1 opcode)
            D[1] = D[2];
            break;
        case 0x53: // Copy D2 into D3 (1 opcode)
            D[3] = D[2];
            break;
        case 0x54: // Copy D2 into D4 (1 opcode)
            D[4] = D[2];
            break;
        case 0x55: // Copy D2 into D5 (1 opcode)
            D[5] = D[2];
            break;
        case 0x56: // Copy D2 into D6 (1 opcode)
            D[6] = D[2];
            break;
        case 0X57: // Copy D3 into D0 (1 opcode)
            D[0] = D[3];
            break;
        case 0X58: // Copy D3 into D1 (1 opcode)
            D[1] = D[3];
            break;
        case 0x59: // Copy D3 into D2 (1 opcode)
            D[2] = D[3];
            break;
        case 0x5A: // Copy D3 into D4 (1 opcode)
            D[4] = D[3];
            break;
        case 0x5B: // Copy D3 into D5 (1 opcode)
            D[5] = D[3];
            break;
        case 0x5C: // Copy D3 into D6 (1 opcode)
            D[6] = D[3];
            break;
        case 0X5D: // Copy D4 into D0 (1 opcode)
            D[0] = D[4];
            break;
        case 0X5E: // Copy D4 into D1 (1 opcode)
            D[1] = D[4];
            break;
        case 0x5F: // Copy D4 into D2 (1 opcode)
            D[2] = D[4];
            break;
        case 0x60: // Copy D4 into D3 (1 opcode)
            D[3] = D[4];
            break;
        case 0x61: // Copy D4 into D5 (1 opcode)
            D[5] = D[4];
            break;
        case 0x62: // Copy D4 into D6 (1 opcode)
            D[6] = D[4];
            break;
        case 0X63: // Copy D5 into D0 (1 opcode)
            D[0] = D[5];
            break;
        case 0X64: // Copy D5 into D1 (1 opcode)
            D[1] = D[5];
            break;
        case 0x65: // Copy D5 into D2 (1 opcode)
            D[2] = D[5];
            break;
        case 0x66: // Copy D5 into D3 (1 opcode)
            D[3] = D[5];
            break;
        case 0x67: // Copy D5 into D4 (1 opcode)
            D[4] = D[5];
            break;
        case 0x68: // Copy D5 into D6 (1 opcode)
            D[6] = D[5];
            break;
        case 0X69: // Copy D6 into D0 (1 opcode)
            D[0] = D[6];
            break;
        case 0X6A: // Copy D6 into D1 (1 opcode)
            D[1] = D[6];
            break;
        case 0x6B: // Copy D6 into D2 (1 opcode)
            D[2] = D[6];
            break;
        case 0x6C: // Copy D6 into D3 (1 opcode)
            D[3] = D[6];
            break;
        case 0x6D: // Copy D6 into D4 (1 opcode)
            D[4] = D[6];
            break;
        case 0x6E: // Copy D6 into D5 (1 opcode)
            D[5] = D[6];
            break;
        case 0x6F: // LDI src=A[0], dst=A[1], length=A[2] (1 opcode)
            memory[A[1] % 0x01000000] = memory[A[0] % 0x01000000];
            A[0] = (A[0] + 1) % 0x01000000;
            A[1] = (A[1] + 1) % 0x01000000;
            A[2] = (A[2] - 1) % 0x01000000;
            break;
        case 0x70: // LDIR src=A[0], dst=A[1], length=A[2] (1 opcode)
            while(A[2] > 0) {
                memory[A[1] % 0x01000000] = memory[A[0] % 0x01000000];
                A[0] = (A[0] + 1) % 0x01000000;
                A[1] = (A[1] + 1) % 0x01000000;
                A[2] = (A[2] - 1) % 0x01000000;
            }
            break;
        case 0x71: // LDD src=A[0], dst=A[1] (1 opcode)
            memory[A[1] % 0x01000000] = memory[A[0] % 0x01000000];
            A[0] = (A[0] - 1) % 0x01000000;
            A[1] = (A[1] - 1) % 0x01000000;
            A[2] = (A[2] - 1) % 0x01000000;
            break;
        case 0x72: // LDDR src=A[0], dst=A[1], length=A[2] (1 opcode)
            while(A[2] > 0) {
                memory[A[1] % 0x01000000] = memory[A[0] % 0x01000000];
                A[0] = (A[0] - 1) % 0x01000000;
                A[1] = (A[1] - 1) % 0x01000000;
                A[2] = (A[2] - 1) % 0x01000000;
            }
            break;
        case 0x73: // Add D0 and D1 (1 opcode)
            // set or reset flags
            D[7] = GetFlagsValue(D[0], D[1], "ADD8");

            // perform 8 bits addition
            D[0] = (D[0] + D[1]) % 256;

            break;
        case 0x74: // Add D0 and D2 (1 opcode)
            // set or reset flags
            D[7] = GetFlagsValue(D[0], D[2], "ADD8");

            // perform 8 bits addition
            D[0] = (D[0] + D[2]) % 256;
            break;
        case 0x75: // Add D0 and D3 (1 opcode)
            // set or reset flags
            D[7] = GetFlagsValue(D[0], D[3], "ADD8");

            // perform 8 bits addition
            D[0] = (D[0] + D[3]) % 256;
            break;
        case 0x76: // Add D0 and D4 (1 opcode)
            // set or reset flags
            D[7] = GetFlagsValue(D[0], D[4], "ADD8");

            // perform 8 bits addition
            D[0] = (D[0] + D[4]) % 256;
            break;
        case 0x77: // Add D0 and D5 (1 opcode)
            // set or reset flags
            D[7] = GetFlagsValue(D[0], D[5], "ADD8");

            // perform 8 bits addition
            D[0] = (D[0] + D[5]) % 256;
            break;
        case 0x78: // Add D0 and D6 (1 opcode)
            // set or reset flags
            D[7] = GetFlagsValue(D[0], D[6], "ADD8");

            // perform 8 bits addition
            D[0] = (D[0] + D[6]) % 256;
            break;
        case 0x79: // Subtract D1 from D0 (1 opcode)
            // set or reset flags
            D[7] = GetFlagsValue(D[0], D[1], "SUB8");

            // perform 8 bits substraction
            D[0] = (D[0] - D[1]) % 256;
            break;
        case 0x7A: // Subtract D2 from D0 (1 opcode)
            // set or reset flags
            D[7] = GetFlagsValue(D[0], D[2], "SUB8");

            // perform 8 bits substraction
            D[0] = (D[0] - D[2]) % 256;
            break;
        case 0x7B: // Subtract D3 from D0 (1 opcode)
            // set or reset flags
            D[7] = GetFlagsValue(D[0], D[3], "SUB8");

            // perform 8 bits substraction
            D[0] = (D[0] - D[3]) % 256;
            break;
        case 0x7C: // Subtract D4 from D0 (1 opcode)
            // set or reset flags
            D[7] = GetFlagsValue(D[0], D[4], "SUB8");

            // perform 8 bits substraction
            D[0] = (D[0] - D[4]) % 256;
            break;
        case 0x7D: // Subtract D5 from D0 (1 opcode)
            // set or reset flags
            D[7] = GetFlagsValue(D[0], D[5], "SUB8");

            // perform 8 bits substraction
            D[0] = (D[0] - D[5]) % 256;
            break;
        case 0x7E: // Subtract D6 from D0 (1 opcode)
            // set or reset flags
            D[7] = GetFlagsValue(D[0], D[6], "SUB8");

            // perform 8 bits substraction
            D[0] = (D[0] - D[6]) % 256;
            break;
        case 0x7F: // Multiply D0 by D1 (1 opcode)
            // set or reset flags
            D[7] = GetFlagsValue(D[0], D[1], "MUL8");

            // perform 8 bits multiplication
            D[0] = (D[0] * D[1]) % 256;
            break;
        case 0x80: // Multiply D0 by D2 (1 opcode)
            // set or reset flags
            D[7] = GetFlagsValue(D[0], D[2], "MUL8");

            // perform 8 bits multiplication
            D[0] = (D[0] * D[2]) % 256;
            break;
        case 0x81: // Multiply D0 by D3 (1 opcode)
            // set or reset flags
            D[7] = GetFlagsValue(D[0], D[3], "MUL8");

            // perform 8 bits multiplication
            D[0] = (D[0] * D[3]) % 256;
            break;
        case 0x82: // Multiply D0 by D4 (1 opcode)
            // set or reset flags
            D[7] = GetFlagsValue(D[0], D[4], "MUL8");

            // perform 8 bits multiplication
            D[0] = (D[0] * D[4]) % 256;
            break;
        case 0x83: // Multiply D0 by D5 (1 opcode)
            // set or reset flags
            D[7] = GetFlagsValue(D[0], D[5], "MUL8");

            // perform 8 bits multiplication
            D[0] = (D[0] * D[5]) % 256;
            break;
        case 0x84: // Multiply D0 by D6 (1 opcode)
            // set or reset flags
            D[7] = GetFlagsValue(D[0], D[6], "MUL8");

            // perform 8 bits multiplication
            D[0] = (D[0] * D[6]) % 256;
            break;
        case 0x85: // Multiply A0 = A0 * A1 (1 opcode)
            // set or reset flags
            D[7] = GetFlagsValue(A[0], A[1], "MUL24");

            // perform 24 bits multiplication
            A[0] = (A[0] * A[1]) % 0x01000000;
            break;
        case 0x86: // Multiply A0 = A0 * D0 (1 opcode)
            // set or reset flags
            D[7] = GetFlagsValue(A[0], D[0], "MUL24");

            // perform 24 bits multiplication
            A[0] = (A[0] * D[0]) % 0x01000000;
            break;
        case 0x87: // Divide A0 = A0 / A1 (1 opcode)
            // set or reset flags
            D[7] = GetFlagsValue(A[0], A[1], "DIV24");

            // perform 24 bits division
            if (A[1] != 0) { A[0] = A[0] / A[1]; } else { A[0] = 0x00FFFFFF; }
            break;
        case 0x88: // Divide A0 = A0 / D0 (1 opcode)
            // set or reset flags
            D[7] = GetFlagsValue(A[0], D[0], "DIV24");

            // perform 16 bits division
            if (D[0] != 0) { A[0] = A[0] / D[0]; } else { A[0] = 0x00FFFFFF; }
            break;
        case 0x89: // Logical And D0 with D1 (1 opcode)
            D[0] = D[0] & D[1];
            break;
        case 0x8A: // Logical And D0 with D2 (1 opcode)
            D[0] = D[0] & D[2];
            break;
        case 0x8B: // Logical And D0 with D3 (1 opcode)
            D[0] = D[0] & D[3];
            break;
        case 0x8C: // Logical And D0 with D4 (1 opcode)
            D[0] = D[0] & D[4];
            break;
        case 0x8D: // Logical And D0 with D5 (1 opcode)
            D[0] = D[0] & D[5];
            break;
        case 0x8E: // Logical And D0 with D6 (1 opcode)
            D[0] = D[0] & D[6];
            break;
        case 0x8F: // Logical Or D0 with D1 (1 opcode)
            D[0] = D[0] | D[1];
            break;
        case 0x90: // Logical Or D0 with D2 (1 opcode)
            D[0] = D[0] | D[2];
            break;
        case 0x91: // Logical Or D0 with D3 (1 opcode)
            D[0] = D[0] | D[3];
            break;
        case 0x92: // Logical Or D0 with D4 (1 opcode)
            D[0] = D[0] | D[4];
            break;
        case 0x93: // Logical Or D0 with D5 (1 opcode)
            D[0] = D[0] | D[5];
            break;
        case 0x94: // Logical Or D0 with D6 (1 opcode)
            D[0] = D[0] | D[6];
            break;
        case 0x95: // Logical Xor D0 with D1 (1 opcode)
            D[0] = D[0] ^ D[1];
            break;
        case 0x96: // Logical Xor D0 with D2 (1 opcode)
            D[0] = D[0] ^ D[2];
            break;
        case 0x97: // Logical Xor D0 with D3 (1 opcode)
            D[0] = D[0] ^ D[3];
            break;
        case 0x98: // Logical Xor D0 with D4 (1 opcode)
            D[0] = D[0] ^ D[4];
            break;
        case 0x99: // Logical Xor D0 with D5 (1 opcode)
            D[0] = D[0] ^ D[5];
            break;
        case 0x9A: // Logical Xor D0 with D6 (1 opcode)
            D[0] = D[0] ^ D[6];
            break;
        case 0x9B: // Logical Not D0 (1 opcode)
            D[0] = !D[0];
            break;
        case 0x9C: // set bit D0.0 (1 opcode)
            D[0] |= (1 << 0);
            break;
        case 0x9D: // set bit D0.1 (1 opcode)
            D[0] |= (1 << 1);
            break;
        case 0x9E: // set bit D0.2 (1 opcode)
            D[0] |= (1 << 2);
            break;
        case 0x9F: // set bit D0.3 (1 opcode)
            D[0] |= (1 << 3);
            break;
        case 0xA0: // set bit D0.4 (1 opcode)
            D[0] |= (1 << 4);
            break;
        case 0xA1: // set bit D0.5 (1 opcode)
            D[0] |= (1 << 5);
            break;
        case 0xA2: // set bit D0.6 (1 opcode)
            D[0] |= (1 << 6);
            break;
        case 0xA3: // set bit D0.7 (1 opcode)
            D[0] |= (1 << 7);
            break;
        case 0xA4: // reset bit D0.0 (1 opcode)
            D[0] &= ~(1 << 0);
            break;
        case 0xA5: // reset bit D0.1 (1 opcode)
            D[0] &= ~(1 << 1);
            break;
        case 0xA6: // reset bit D0.2 (1 opcode)
            D[0] &= ~(1 << 2);
            break;
        case 0xA7: // reset bit D0.3 (1 opcode)
            D[0] &= ~(1 << 3);
            break;
        case 0xA8: // reset bit D0.4 (1 opcode)
            D[0] &= ~(1 << 4);
            break;
        case 0xA9: // reset bit D0.5 (1 opcode)
            D[0] &= ~(1 << 5);
            break;
        case 0xAA: // reset bit D0.6 (1 opcode)
            D[0] &= ~(1 << 6);
            break;
        case 0xAB: // reset bit D0.7 (1 opcode)
            D[0] &= ~(1 << 7);
            break;
        case 0xAC: // test bit D0.0 in Zero flag (1 opcode)
            {
                uint8_t bit = D[0] & (1 << 0);

                if(bit == 0) {
                    D[7] |= (1 << FlagZ);
                } else {
                    D[7] &= ~(1 << FlagZ);
                }
            }
            break;
        case 0xAD: // test bit D0.1 in Zero flag (1 opcode)
            {
                uint8_t bit = D[0] & (1 << 1);

                if(bit == 0) {
                    D[7] |= (1 << FlagZ);
                } else {
                    D[7] &= ~(1 << FlagZ);
                }
            }
            break;
        case 0xAE: // test bit D0.2 in Zero flag (1 opcode)
            {
                uint8_t bit = D[0] & (1 << 2);

                if(bit == 0) {
                    D[7] |= (1 << FlagZ);
                } else {
                    D[7] &= ~(1 << FlagZ);
                }
            }
            break;
        case 0xAF: // test bit D0.3 in Zero flag (1 opcode)
            {
                uint8_t bit = D[0] & (1 << 3);

                if(bit == 0) {
                    D[7] |= (1 << FlagZ);
                } else {
                    D[7] &= ~(1 << FlagZ);
                }
            }
            break;
        case 0xB0: // test bit D0.4 in Zero flag (1 opcode)
            {
                uint8_t bit = D[0] & (1 << 4);

                if(bit == 0) {
                    D[7] |= (1 << FlagZ);
                } else {
                    D[7] &= ~(1 << FlagZ);
                }
            }
            break;
        case 0xB1: // test bit D0.5 in Zero flag (1 opcode)
            {
                uint8_t bit = D[0] & (1 << 5);

                if(bit == 0) {
                    D[7] |= (1 << FlagZ);
                } else {
                    D[7] &= ~(1 << FlagZ);
                }
            }
            break;
        case 0xB2: // test bit D0.6 in Zero flag (1 opcode)
            {
                uint8_t bit = D[0] & (1 << 6);

                if(bit == 0) {
                    D[7] |= (1 << FlagZ);
                } else {
                    D[7] &= ~(1 << FlagZ);
                }
            }
            break;
        case 0xB3: // test bit D0.7 in Zero flag (1 opcode)
            {
                uint8_t bit = D[0] & (1 << 7);

                if(bit == 0) {
                    D[7] |= (1 << FlagZ);
                } else {
                    D[7] &= ~(1 << FlagZ);
                }
            }
            break;
        case 0xB4: // Push A0 onto Stack (1 opcode)
            stack[SP] = A[0];
            SP--;
            break;
        case 0xB5: // Push A1 onto Stack (1 opcode)
            stack[SP] = A[1];
            SP--;
            break;
        case 0xB6: // Push A2 onto Stack (1 opcode)
            stack[SP] = A[2];
            SP--;
            break;
        case 0xB7: // Push A3 onto Stack (1 opcode)
            stack[SP] = A[3];
            SP--;
            break;
        case 0xB8: // Push A4 onto Stack (1 opcode)
            stack[SP] = A[3];
            SP--;
            break;
        case 0xB9: // Push A5 onto Stack (1 opcode)
            stack[SP] = A[5];
            SP--;
            break;
        case 0xBA: // Push A6 onto Stack (1 opcode)
            stack[SP] = A[6];
            SP--;
            break;
        case 0xBB: // Push A7 onto Stack (1 opcode)
            stack[SP] = A[7];
            SP--;
            break;
        case 0xBC: // Push D0/D1/D2 onto Stack (1 opcode)
            stack[SP] = ((uint32_t)D[0] << 16) | ((uint32_t)D[1] << 8) | (uint32_t)D[2];
            SP--;
            break;
        case 0xBD: // Push D1/D2/D3 onto Stack (1 opcode)
            stack[SP] = ((uint32_t)D[1] << 16) | ((uint32_t)D[2] << 8) | (uint32_t)D[3];
            SP--;
            break;
        case 0xBE: // Push D2/D3/D4 onto Stack (1 opcode)
            stack[SP] = ((uint32_t)D[2] << 16) | ((uint32_t)D[3] << 8) | (uint32_t)D[4];
            SP--;
            break;
        case 0xBF: // Push D3/D4/D5 onto Stack (1 opcode)
            stack[SP] = ((uint32_t)D[3] << 16) | ((uint32_t)D[4] << 8) | (uint32_t)D[5];
            SP--;
            break;
        case 0xC0: // Pop Stack into A0 (1 opcode)
            SP++;
            A[0] = stack[SP];
            break;
        case 0xC1: // Pop Stack into A1 (1 opcode)
            SP++;
            A[1] = stack[SP];
            break;
        case 0xC2: // Pop Stack into A2 (1 opcode)
            SP++;
            A[2] = stack[SP];
            break;
        case 0xC3: // Pop Stack into A3 (1 opcode)
            SP++;
            A[3] = stack[SP];
            break;
        case 0xC4: // Pop Stack into A4 (1 opcode)
            SP++;
            A[4] = stack[SP];
            break;
        case 0xC5: // Pop Stack into A5 (1 opcode)
            SP++;
            A[5] = stack[SP];
            break;
        case 0xC6: // Pop Stack into A6 (1 opcode)
            SP++;
            A[6] = stack[SP];
            break;
        case 0xC7: // Pop Stack into A7 (1 opcode)
            SP++;
            A[7] = stack[SP];
            break;
        case 0xC8: // Pop Stack into D0/D1/D2 (1 opcode)
            SP++;
            D[0] = (uint8_t)(stack[SP] >> 16);
            D[1] = (uint8_t)(stack[SP] >> 8);
            D[2] = (uint8_t)(stack[SP] & 0xFF);
            break;
        case 0xC9: // Pop Stack into D1/D2/D3 (1 opcode)
            SP++;
            D[1] = (uint8_t)(stack[SP] >> 16);
            D[2] = (uint8_t)(stack[SP] >> 8);
            D[3] = (uint8_t)(stack[SP] & 0xFF);
            break;
        case 0xCA: // Pop Stack into D2/D3/D4 (1 opcode)
            SP++;
            D[2] = (uint8_t)(stack[SP] >> 16);
            D[3] = (uint8_t)(stack[SP] >> 8);
            D[4] = (uint8_t)(stack[SP] & 0xFF);
            break;
        case 0xCB: // Pop Stack into D3/D4/D5 (1 opcode)
            SP++;
            D[3] = (uint8_t)(stack[SP] >> 16);
            D[4] = (uint8_t)(stack[SP] >> 8);
            D[5] = (uint8_t)(stack[SP] & 0xFF);
            break;
        case 0xCC: // Shift left D0 (1 opcode)
            // set or reset flags
            D[7] = GetFlagsValue(D[0], 0, "SL8");

            // shift left
            D[0] = D[0] << 1;
            break;
        case 0xCD: // Shift right D0 (1 opcode)
            // set or reset flags
            D[7] = GetFlagsValue(D[0], 0, "SR8");

            // shift right
            D[0] = D[0] >> 1;
            break;
        case 0xCE: // Roll left D0 (1 opcode)
            {
                // set or reset flags
                D[7] = GetFlagsValue(D[0], 0, "RL8");

                // shift left
                uint8_t bit = ((D[0] & 0x80) ? 1 : 0);

                D[0] = (D[0] << 1) | bit;
            }
            break;
        case 0xCF: // Roll right D0 (1 opcode)
            {
                // set or reset flags
                D[7] = GetFlagsValue(D[0], 0, "RR8");

                // shift left
                uint8_t bit = ((D[0] & 0x01) ? 0x80 : 0);

                D[0] = (D[0] >> 1) | bit;
            }
            break;
        case 0xD0: // Increment D0 (1 opcode)
            // set or reset flags
            D[7] = GetFlagsValue(D[0], 1, "INC8");

            // perform 8 bits incrementation
            D[0]++;
            break;
        case 0xD1: // Increment D1 (1 opcode)
            // set or reset flags
            D[7] = GetFlagsValue(D[1], 1, "INC8");

            // perform 8 bits incrementation
            D[1]++;
            break;
        case 0xD2: // Increment D2 (1 opcode)
            // set or reset flags
            D[7] = GetFlagsValue(D[2], 1, "INC8");

            // perform 8 bits incrementation
            D[2]++;
            break;
        case 0xD3: // Increment D3 (1 opcode)
            // set or reset flags
            D[7] = GetFlagsValue(D[3], 1, "INC8");

            // perform 8 bits incrementation
            D[3]++;
            break;
        case 0xD4: // Increment D4 (1 opcode)
            // set or reset flags
            D[7] = GetFlagsValue(D[4], 1, "INC8");

            // perform 8 bits incrementation
            D[4]++;
            break;
        case 0xD5: // Increment D5 (1 opcode)
            // set or reset flags
            D[7] = GetFlagsValue(D[5], 1, "INC8");

            // perform 8 bits incrementation
            D[5]++;
            break;
        case 0xD6: // Increment D6 (1 opcode)
            // set or reset flags
            D[7] = GetFlagsValue(D[6], 1, "INC8");

            // perform 8 bits incrementation
            D[6]++;
            break;
        case 0xD7: // Decrement D0 (1 opcode)
            // set or reset flags
            D[7] = GetFlagsValue(D[0], 1, "DEC8");

            // perform 8 bits decrementation
            D[0]--;
            break;
        case 0xD8: // Decrement D1 (1 opcode)
            // set or reset flags
            D[7] = GetFlagsValue(D[1], 1, "DEC8");

            // perform 8 bits decrementation
            D[1]--;
            break;
        case 0xD9: // Decrement D2 (1 opcode)
            // set or reset flags
            D[7] = GetFlagsValue(D[2], 1, "DEC8");

            // perform 8 bits decrementation
            D[2]--;
            break;
        case 0xDA: // Decrement D3 (1 opcode)
            // set or reset flags
            D[7] = GetFlagsValue(D[3], 1, "DEC8");

            // perform 8 bits decrementation
            D[3]--;
            break;
        case 0xDB: // Decrement D4 (1 opcode)
            // set or reset flags
            D[7] = GetFlagsValue(D[4], 1, "DEC8");

            // perform 8 bits decrementation
            D[4]--;
            break;
        case 0xDC: // Decrement D5 (1 opcode)
            // set or reset flags
            D[7] = GetFlagsValue(D[5], 1, "DEC8");

            // perform 8 bits decrementation
            D[5]--;
            break;
        case 0xDD: // Decrement D6 (1 opcode)
            // set or reset flags
            D[7] = GetFlagsValue(D[6], 1, "DEC8");

            // perform 8 bits decrementation
            D[6]--;
            break;
        case 0xDE: // Copy D0,D1,D2 to A0 (1 opcode)
            A[0] = ((uint32_t)(D[0] << 16)) | (uint32_t(D[1] << 8)) | (uint32_t)D[2];
            break;
        case 0xDF: // Increment A0 (1 opcode)
            // set or reset flags
            D[7] = GetFlagsValue(A[0], 1, "INC24");

            // perform 16 bits incrementation
            A[0]++;
            break;
        case 0xE0: // Increment A1 (1 opcode)
            // set or reset flags
            D[7] = GetFlagsValue(A[1], 1, "INC24");

            // perform 16 bits incrementation
            A[1]++;
            break;
        case 0xE1: // Increment A2 (1 opcode)
            // set or reset flags
            D[7] = GetFlagsValue(A[2], 1, "INC24");

            // perform 16 bits incrementation
            A[2]++;
            break;
        case 0xE2: // Increment A3 (1 opcode)
            // set or reset flags
            D[7] = GetFlagsValue(A[3], 1, "INC24");

            // perform 16 bits incrementation
            A[3]++;
            break;
        case 0xE3: // Increment A4 (1 opcode)
            // set or reset flags
            D[7] = GetFlagsValue(A[4], 1, "INC24");

            // perform 16 bits incrementation
            A[4]++;
            break;
        case 0xE4: // Increment A5 (1 opcode)
            // set or reset flags
            D[7] = GetFlagsValue(A[5], 1, "INC24");

            // perform 16 bits incrementation
            A[5]++;
            break;
        case 0xE5: // Increment A6 (1 opcode)
            // set or reset flags
            D[7] = GetFlagsValue(A[6], 1, "INC24");

            // perform 16 bits incrementation
            A[6]++;
            break;
        case 0xE6: // Increment A7 (1 opcode)
            // set or reset flags
            D[7] = GetFlagsValue(A[7], 1, "INC24");

            // perform 16 bits incrementation
            A[7]++;
            break;
        case 0xE7: // Decrement A0 (1 opcode)
            // set or reset flags
            D[7] = GetFlagsValue(A[0], 1, "DEC24");

            // perform 16 bits decrementation
            A[0]--;
            break;
        case 0xE8: // Decrement A1 (1 opcode)
            // set or reset flags
            D[7] = GetFlagsValue(A[1], 1, "DEC24");

            // perform 16 bits decrementation
            A[1]--;
            break;
        case 0xE9: // Decrement A2 (1 opcode)
            // set or reset flags
            D[7] = GetFlagsValue(A[2], 1, "DEC24");

            // perform 16 bits decrementation
            A[2]--;
            break;
        case 0xEA: // Decrement A3 (1 opcode)
            // set or reset flags
            D[7] = GetFlagsValue(A[3], 1, "DEC24");

            // perform 16 bits decrementation
            A[3]--;
            break;
        case 0xEB: // Decrement A4 (1 opcode)
            // set or reset flags
            D[7] = GetFlagsValue(A[4], 1, "DEC24");

            // perform 16 bits decrementation
            A[4]--;
            break;
        case 0xEC: // Decrement A5 (1 opcode)
            // set or reset flags
            D[7] = GetFlagsValue(A[5], 1, "DEC24");

            // perform 16 bits decrementation
            A[5]--;
            break;
        case 0xED: // Decrement A6 (1 opcode)
            // set or reset flags
            D[7] = GetFlagsValue(A[6], 1, "DEC24");

            // perform 16 bits decrementation
            A[6]--;
            break;
        case 0xEE: // Decrement A7 (1 opcode)
            // set or reset flags
            D[7] = GetFlagsValue(A[7], 1, "DEC24");

            // perform 16 bits decrementation
            A[7]--;
            break;
        case 0xEF: // Compare A0 with Immediate Value (2 opcode)
            {
                uint32_t addr = (getOpcode() << 16) | (getOpcode() << 8) | getOpcode();

                // set or reset flags
                D[7] = GetFlagsValue(A[0], addr, "CP24");
            }
            break;
        case 0xFF:
            {
                uint8_t newOpcode = getOpcode();

                switch(newOpcode) {
                    case 0x00:
                        ClearScreen();
                        break;
                    case 0x01: // Draw Sprite (5 opcodes)
                        {
                            uint32_t x = A[0];
                            uint32_t y = (uint32_t)D[0];
                            uint8_t width = D[1];
                            uint8_t height = D[2];
                            uint32_t spriteAddress = A[1];
                            DrawSprite(x, y, width, height, spriteAddress);
                        }
                        break;
                    case 0x02: // Draw Block from Screen to Memory (5 opcodes)
                        {
                            uint32_t x = A[0];
                            uint32_t y = (uint32_t)D[0];
                            uint8_t width = D[1];
                            uint8_t height = D[2];
                            uint32_t spriteAddress = A[1];
                            DrawBlock(x, y, width, height, spriteAddress);
                        }
                        break;
                    case 0x03: // Grab Block from Memory to Screen (5 opcodes)
                        {
                            uint32_t x = A[0];
                            uint32_t y = (uint32_t)D[0];
                            uint8_t width = D[1];
                            uint8_t height = D[2];
                            uint32_t spriteAddress = A[1];
                            GrabBlock(x, y, width, height, spriteAddress);
                        }
                        break;
                    case 0x04: // Get Joystick State in D0 (1 opcode)
                        D[0] = joystick;
                        break;
                    case 0x05: // Locate Cursor At X,Y (3 opcodes)
                        {
                            uint8_t x = getOpcode();
                            uint8_t y = getOpcode();
                
                            D[7] = Locate(x, y);
                        }
                        break;
                    case 0x06: // Print Character In D0 (1 opcode)
                        D[7] = PrintChar(D[0]);
                        break;
                    case 0x07: // Print a Text (3 opcodes)
                        {
                            uint8_t txtAddrHIGH = getOpcode();
                            uint8_t txtAddrMID = getOpcode();
                            uint8_t txtAddrLOW = getOpcode();
                            uint32_t txtAddress = (txtAddrHIGH << 16) | (txtAddrMID << 8) | txtAddrLOW;
                            D[7] = PrintString(txtAddress);
                        }
                        break;
                    case 0x08: // Set Pen Color (2 opcode)
                        pen = getOpcode();
                        break;
                    case 0x09: // Set Paper Color (2 opcode)
                        paper = getOpcode();
                        break;
                    case 0x0A: // Set Border Color (2 opcode)
                        border = getOpcode();
                        break;
                    case 0x0B: // Set a pixel with Color D1 at A0, D0 (1 opcode)
                        SetPixel(A[0], (uint32_t)D[0], D[1]);
                        break;
                    case 0x0C: // Get pixel color at A0, D0 in D1 (1 opcode)
                        D[1] = GetPixel(A[0], (uint32_t)D[0]);
                        break;
                    case 0x0D: // Scroll Screen (3 opcode)
                        {
                            uint8_t scrollx = getOpcode();
                            uint8_t scrolly = getOpcode();

                            ScrollScreen(scrollx, scrolly);
                        }
                        break;
                    case 0x0E: // Flip Screen (1 opcode)
                        Flip();
                        break;
                    case 0x0F: // Play Sound (5 opcodes)
                        {
                            uint8_t sndAddrHIGH = getOpcode();
                            uint8_t sndAddrMID = getOpcode();
                            uint8_t sndAddrLOW = getOpcode();
                            uint32_t soundAddress = (sndAddrHIGH << 16) | (sndAddrMID << 8) | sndAddrLOW;
                            uint8_t sndLenHIGH = getOpcode();
                            uint8_t sndLenMID = getOpcode();
                            uint8_t sndLenLOW = getOpcode();
                            uint32_t soundLength = (sndLenHIGH << 16) | (sndLenMID << 8) | sndLenLOW;

                            PlaySound(soundAddress, soundLength);
                        }
                        break;
                    case 0x10: // Play Music In Audio Memory (5 opcodes)
                        {
                            uint8_t sndAddrHIGH = getOpcode();
                            uint8_t sndAddrMID = getOpcode();
                            uint8_t sndAddrLOW = getOpcode();
                            uint32_t soundAddress = (sndAddrHIGH << 16) | (sndAddrMID << 8) | sndAddrLOW;
                            uint8_t sndLenHIGH = getOpcode();
                            uint8_t sndLenMID = getOpcode();
                            uint8_t sndLenLOW = getOpcode();
                            uint32_t soundLength = (sndLenHIGH << 16) | (sndLenMID << 8) | sndLenLOW;

                            PlayMusic(soundAddress, soundLength);
                        }
                        break;
                    case 0x11: // Stop Music playback (1 opcode)
                        StopMusic();
                        break;
                }
            }
            break;
        default:
            cout << "Unknown opcode: " << opcode << endl;
    }
}

void Monster8::DrawSprite(uint32_t x, uint32_t y, uint8_t w, uint8_t h, uint32_t addr) {
    for(uint32_t yscreen = 0; yscreen < h; ++yscreen) {
        for(uint32_t xscreen = 0; xscreen < w; ++xscreen) {
            if(sprites[addr] > 0) {
                SetPixel(x + xscreen,y + yscreen, sprites[addr]);
            }

            addr++;
        }
    }
}

void Monster8::DrawBlock(uint32_t x, uint32_t y, uint8_t w, uint8_t h, uint32_t addr) {
    for(uint32_t yscreen = 0; yscreen < h; ++yscreen) {
        for(uint32_t xscreen = 0; xscreen < w; ++xscreen) {
            SetPixel(x + xscreen,y + yscreen, memory[addr]);

            addr++;
        }
    }
}

void Monster8::GrabBlock(uint32_t x, uint32_t y, uint8_t w, uint8_t h, uint32_t addr) {
    for(uint32_t yscreen = 0; yscreen < h; ++yscreen) {
        for(uint32_t xscreen = 0; xscreen < w; ++xscreen) {
            memory[addr] = GetPixel(x + xscreen,y + yscreen);

            addr++;
        }
    }
}

uint8_t Monster8::getJoystickState() {
    return joystick;
}

void Monster8::PlaySound(uint32_t addr, uint32_t length) {
    int qfreq; Uint16 qfmt; int qch;
    if (Mix_QuerySpec(&qfreq, &qfmt, &qch) == 0) {
        return;
    }

    if (addr >= audio1.size() || addr + length > audio1.size()) {
        return;
    }

    // Stop and free previous sound if any
    if (sound != NULL) {
        Mix_HaltChannel(-1);
        Mix_FreeChunk(sound);
        sound = NULL;
    }

    SDL_RWops* rw = SDL_RWFromConstMem(audio1.data() + addr, static_cast<int>(length));
    if (!rw) {
        return;
    }

    Mix_Chunk* chunk = Mix_LoadWAV_RW(rw, 1); // frees rw
    if (!chunk) {
        return;
    }

    sound = chunk;
    Mix_Volume(-1, MIX_MAX_VOLUME);

    (void)Mix_PlayChannel(-1, sound, 0);
}

void Monster8::PlayMusic(uint32_t addr, uint32_t length) {
    int qfreq; Uint16 qfmt; int qch;
    if (Mix_QuerySpec(&qfreq, &qfmt, &qch) == 0) {
        return;
    }

    if (addr >= audio2.size() || addr + length > audio2.size()) {
        return;
    }

    if (music != NULL) {
        Mix_HaltMusic();
        Mix_FreeMusic(music);
        music = NULL;
    }

    SDL_RWops* rw = SDL_RWFromConstMem(audio2.data() + addr, static_cast<int>(length));
    if (!rw) {
        return;
    }

    Mix_Music* mus = Mix_LoadMUS_RW(rw, 1); // frees rw
    if (!mus) {
        return;
    }

    music = mus;
    (void)Mix_PlayMusic(music, -1);
}

void Monster8::StopMusic() {
    // Stop current music and free resources (only if playing or fading)
    if (Mix_PlayingMusic() || Mix_FadingMusic() != MIX_NO_FADING) {
        Mix_HaltMusic();
    }
    if (music != NULL) {
        Mix_FreeMusic(music);
        music = NULL;
    }
}

uint8_t Monster8::Locate(uint8_t x, uint8_t y) {
    if((x >= 1) && (x <= 40) && (y >= 1) && (y <= 25)) {
        cursorX = x;
        cursorY = y;

        return 0x00;
    }

    return 0xFF;
}

// print a character
uint8_t Monster8::PrintChar(char c) {
    // Gestion du retour à la ligne
    if (c == '\n') {
        cursorX = 1;
        cursorY = static_cast<uint8_t>(cursorY + 1);
        if (cursorY > 25) {
            cursorY = 25;

            return 0xFF;
        }
    }

    // Dessine un caractère 8x8 à partir de la police VGA
    uint32_t x0 = static_cast<uint32_t>((cursorX - 1) * 8);
    uint32_t y0 = static_cast<uint32_t>((cursorY - 1) * 8);

    if (x0 >= 320 || y0 >= 200) return 0xFF; // Hors écran, ignorer

    uint8_t uc = static_cast<uint8_t>(c);
    for (uint32_t row = 0; row < 8; ++row) {
        if (y0 + row >= 200) break;
        uint8_t pattern = fontset[uc][row];
        for (uint32_t col = 0; col < 8; ++col) {
            if (x0 + col >= 320) break;
            // bit 0 = colonne 0 (LSB à gauche)
            if (pattern & (1u << col)) {
                SetPixel(static_cast<uint32_t>(x0 + col), static_cast<uint32_t>(y0 + row), pen);
            }
        }
    }

    // Avancer le curseur et gérer le retour à la ligne automatique
    cursorX = static_cast<uint8_t>(cursorX + 1);
    if (cursorX > 40) { // 320/8 = 40 colonnes
        cursorX = 1;
        cursorY = static_cast<uint8_t>(cursorY + 1);
        if (cursorY > 25) {
            cursorY = 25;

            return 0xFF;
        }
    }

    return 0x00;
}

uint8_t Monster8::PrintString(uint32_t addr) {
    while(memory[addr] != 0 && addr < 0x01000000) {
        if(PrintChar(memory[addr]) != 0xFF) {
            addr++;
        } else {
            return 0xFF;
        }
    }

    return 0x00;
}

void Monster8::SetPixel(uint32_t x, uint32_t y, uint8_t c) {
    // Ecrit un pixel (1 octet couleur) dans le buffer 320x200 et déclenche le redraw
    if (x < 320 && y < 200) {
        screen[(y * 320) + x] = c;
    }
}

uint8_t Monster8::GetPixel(uint32_t x, uint32_t y) {
    return screen[(y * 320) + x];
}

void Monster8::ScrollScreen(uint8_t scrollX, uint8_t scrollY) {
    // Interpréter scrollX/scrollY comme signés (>= 0x80 => négatif)
    // négatif -> haut ou gauche, positif -> bas ou droite
    int dx = (scrollX >= 0x80) ? static_cast<int>(scrollX) - 256 : static_cast<int>(scrollX);
    int dy = (scrollY >= 0x80) ? static_cast<int>(scrollY) - 256 : static_cast<int>(scrollY);
    
    // Scrolling horizontal
    if(dx > 0) {
        for(int y = 0 ; y < 200; y++) {
            for(int x = 319 - dx ; x >= 0; x--) {
                screen[(y * 320) + x + dx] = screen[(y * 320) + x];
            }
        }
    } else if (dx < 0){
        for(int y = 0 ; y < 200; y++) {
            for(int x = -dx ; x <= 319; x++) {
                screen[(y * 320) + x + dx] = screen[(y * 320) + x];
            }
        }
    }

    // Scrolling vertical
    if(dy > 0) {
        for(int x = 0 ; x < 320; x++) {
            for(int y = 199 - dy ; y >= 0; y--) {
                screen[((y + dy) * 320) + x] = screen[(y * 320) + x];
            }
        }
    } else if (dy < 0){
        for(int x = 0 ; x < 320; x++) {
            for(int y = -dy ; y <= 199; y++) {
                screen[((y + dy) * 320) + x] = screen[(y * 320) + x];
            }
        }
    }
}

void Monster8::Flip() {
    // Demander un rafraîchissement de l'écran
    drawFlag = true;
}

void Monster8::WriteString(uint32_t addr, const char* str) {
    // Ecrit une chaîne C dans la mémoire émulée, terminée par 0
    if (!str) return;
    while (*str && addr < 0x01000000) {
        memory[addr++] = static_cast<uint8_t>(*str++);
    }
    if (addr < 0x01000000) memory[addr] = 0; // null-terminator
}

void Monster8::ClearScreen() {
    // Clear only the framebuffer (320x200=64000 bytes), keep palettes intact
    for (int i = 0; i < 320 * 200; ++i) {
        screen[i] = paper;
    }
}

uint8_t Monster8::GetFlagsValue(uint32_t v1, uint32_t v2, string v3) {
    if(v3 == "ADD8") {
        // set or reset PV flag (oVerflow flag)
        if((v1 + v2) > 0xFF) { D[7] |= (1 << FlagPV); } else { D[7] &= ~(1 << FlagPV); }

        // set or reset flag C
        if((v1 + v2) > 0xFF) { D[7] |= (1 << FlagC); } else { D[7] &= ~(1 << FlagC); }

        // set or reset flag S
        if((v1 + v2) > 0x7F) { D[7] |= (1 << FlagS); } else { D[7] &= ~(1 << FlagS); }

        // set or reset flag Z
        if((v1 + v2) % 256 == 0x00) { D[7] |= (1 << FlagZ); } else { D[7] &= ~(1 << FlagZ); }

        // set or reset flag H
        if((v1 + v2) > 0x0F) { D[7] |= (1 << FlagH); } else { D[7] &= ~(1 << FlagH); }

        // reset flag N
        D[7] &= ~(1 << FlagN);
    } else if(v3 == "SUB8") {
        // set or reset PV flag (oVerflow flag)
        if(v1 < v2) { D[7] |= (1 << FlagPV); } else { D[7] &= ~(1 << FlagPV); }

        // set or reset flag C
        if(v1 < v2) { D[7] |= (1 << FlagC); } else { D[7] &= ~(1 << FlagC); }

        // set or reset flag S
        if(((v1 - v2) % 256) > 0x7F) { D[7] |= (1 << FlagS); } else { D[7] &= ~(1 << FlagS); }

        // set or reset flag Z
        if((v1 - v2) % 256 == 0x00) { D[7] |= (1 << FlagZ); } else { D[7] &= ~(1 << FlagZ); }

        // set or reset flag H
        if(((v1 - v2) % 256) > 0x0F) { D[7] |= (1 << FlagH); } else { D[7] &= ~(1 << FlagH); }

        // set flag N
        D[7] |= (1 << FlagN);
    } else if(v3 == "MUL8") {
        // set or reset PV flag (oVerflow flag)
        if((v1 * v2) > 0xFF) { D[7] |= (1 << FlagPV); } else { D[7] &= ~(1 << FlagPV); }

        // set or reset flag C
        if((v1 * v2) > 0xFF) { D[7] |= (1 << FlagC); } else { D[7] &= ~(1 << FlagC); }

        // set or reset flag S
        if((v1 * v2) > 0x7F) { D[7] |= (1 << FlagS); } else { D[7] &= ~(1 << FlagS); }

        // set or reset flag Z
        if((v1 * v2) % 256 == 0x00) { D[7] |= (1 << FlagZ); } else { D[7] &= ~(1 << FlagZ); }

        // set or reset flag H
        if((v1 * v2) > 0x0F) { D[7] |= (1 << FlagH); } else { D[7] &= ~(1 << FlagH); }

        // reset flag N
        D[7] &= ~(1 << FlagN);
    } else if(v3 == "MUL24") {
        // set or reset PV flag (oVerflow flag)
        if((v1 * v2) > 0xFFFFFF) { D[7] |= (1 << FlagPV); } else { D[7] &= ~(1 << FlagPV); }

        // set or reset flag C
        if((v1 * v2) > 0xFFFFFF) { D[7] |= (1 << FlagC); } else { D[7] &= ~(1 << FlagC); }

        // set or reset flag S
        if((v1 * v2) > 0x7FFFFF) { D[7] |= (1 << FlagS); } else { D[7] &= ~(1 << FlagS); }

        // set or reset flag Z
        if((v1 * v2) % 0x01000000 == 0x000000) { D[7] |= (1 << FlagZ); } else { D[7] &= ~(1 << FlagZ); }

        // set or reset flag H
        if((v1 * v2) > 0x000FFFFF) { D[7] |= (1 << FlagH); } else { D[7] &= ~(1 << FlagH); }

        // reset flag N
        D[7] &= ~(1 << FlagN);
    } else if(v3 == "DIV24") {
        if(v2 == 0) {
            D[7] = 0xFF; // division by zero
        } else {
            D[7] = 0x00;
        }
    } else if(v3 == "INC8") {
        // set or reset PV flag (oVerflow flag)
        if(v1 < v2) { D[7] |= (1 << FlagPV); } else { D[7] &= ~(1 << FlagPV); }

        // set or reset flag S
        if((v1 + v2) > 0x7F) { D[7] |= (1 << FlagS); } else { D[7] &= ~(1 << FlagS); }

        // set or reset flag Z
        if((v1 + v2) % 256 == 0x00) { D[7] |= (1 << FlagZ); } else { D[7] &= ~(1 << FlagZ); }

        // set or reset flag H
        if((v1 + v2) > 0x0F) { D[7] |= (1 << FlagH); } else { D[7] &= ~(1 << FlagH); }

        // reset flag N
        D[7] &= ~(1 << FlagN);
    } else if(v3 == "DEC8") {
        // set or reset PV flag (oVerflow flag)
        if(v1 < v2) { D[7] |= (1 << FlagPV); } else { D[7] &= ~(1 << FlagPV); }

        // set or reset flag S
        if(((v1 - v2) % 256) > 0x7F) { D[7] |= (1 << FlagS); } else { D[7] &= ~(1 << FlagS); }

        // set or reset flag Z
        if((v1 - v2) % 256 == 0x00) { D[7] |= (1 << FlagZ); } else { D[7] &= ~(1 << FlagZ); }

        // set or reset flag H
        if(((v1 - v2) % 256) > 0x0F) { D[7] |= (1 << FlagH); } else { D[7] &= ~(1 << FlagH); }

        // set flag N
        D[7] |= (1 << FlagN);
    } else if(v3 == "CP8") {
        // set or reset PV flag (oVerflow flag)
        if(v1 < v2) { D[7] |= (1 << FlagPV); } else { D[7] &= ~(1 << FlagPV); }

        // set or reset flag C
        if(v1 < v2) { D[7] |= (1 << FlagC); } else { D[7] &= ~(1 << FlagC); }

        // set or reset flag S
        if(((v1 - v2) % 256) > 0x7F) { D[7] |= (1 << FlagS); } else { D[7] &= ~(1 << FlagS); }

        // set or reset flag Z
        if((v1 - v2) % 256 == 0x00) { D[7] |= (1 << FlagZ); } else { D[7] &= ~(1 << FlagZ); }

        // set or reset flag H
        if(((v1 - v2) % 256) > 0x0F) { D[7] |= (1 << FlagH); } else { D[7] &= ~(1 << FlagH); }

        // set flag N
        D[7] |= (1 << FlagN);
    } else if(v3 == "CP24") {
        // set or reset PV flag (oVerflow flag)
        if(v1 < v2) { D[7] |= (1 << FlagPV); } else { D[7] &= ~(1 << FlagPV); }

        // set or reset flag C
        if(v1 < v2) { D[7] |= (1 << FlagC); } else { D[7] &= ~(1 << FlagC); }

        // set or reset flag S
        if(((v1 - v2) % 0x1000000 ) > 0x7FFFFF) { D[7] |= (1 << FlagS); } else { D[7] &= ~(1 << FlagS); }

        // set or reset flag Z
        if((v1 - v2) % 0x1000000 == 0x000000) { D[7] |= (1 << FlagZ); } else { D[7] &= ~(1 << FlagZ); }

        // set or reset flag H
        if(((v1 - v2) % 0x1000000) > 0x0FFFFF) { D[7] |= (1 << FlagH); } else { D[7] &= ~(1 << FlagH); }

        // set flag N
        D[7] |= (1 << FlagN);
    } else if(v3 == "SL8") {
        // todo
    } else if(v3 == "SR8") {
        // todo!
    } else if(v3 == "RL8") {
        // todo!
    } else if(v3 == "RR8") {
        // todo!
    }

    return D[7];
}