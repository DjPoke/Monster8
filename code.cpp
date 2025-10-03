#include "code.hpp"


ApplyCode::ApplyCode()
    :   spritesheetCount(0),
        tilesetCount(0),
        soundCount(0),
        musicCount(0),
        pass(0),
        lastLabelLine(0),
          PC(0x400),
        asmFile(""),
        binFile(""),
        emuFile(""),
        statusBarUpdater(nullptr)
{
    spritesheetPath.clear();
    spriteAddress.clear();
    spriteLength.clear();
    spriteWidth.clear();
    spriteHeight.clear();
    tilesetPath.clear();
    tilesetAddress.clear();
    tilesetLength.clear();
    tileWidth.clear();
    tileHeight.clear();
    soundPath.clear();
    soundAddress.clear();
    soundLength.clear();
    musicPath.clear();
    musicAddress.clear();
    musicLength.clear();

    std::memset(memory, 0x00, sizeof(memory));

    for (auto &labelEntry : label) {
        labelEntry[0].clear();
        labelEntry[1].clear();
    }

    // lambda vide pour éviter bad_function_call
    statusBarUpdater = [](const std::string&) { /* ne fait rien */ };
}

bool ApplyCode::ToMemory(uint32_t a, uint8_t d) {
    if(a >= 0 && a < 0x1000000 && d >= 0 && d <= 255) {
        if(pass == 2) {
            memory[a] = d;
        }

        PC++;
    } else {
        if (statusBarUpdater != nullptr) {
            statusBarUpdater("Bad Address or Data!");

            return false;
        }
    }

    return true;
}

bool ApplyCode::CheckGarbages(std::string s) {
    if(s == "") { return false; }

    if(s[0] == ';') { return false; }

    return true;
}

uint32_t ApplyCode::FindLabelAddress(std::string s, uint32_t l) {
    if(pass == 2) {
        l = lastLabelLine;
    }

    for(uint32_t i = 0; i <= l; i++) {
        if(s == label[i][0]) {
            return Val(label[i][1]);
        }
    }

    return 0;
}

void ApplyCode::WriteLabelOrValue(uint32_t r, std::string dat, uint32_t l) {
    uint32_t addr = FindLabelAddress(StringField(dat, r, ","), l);
      
    if(addr > 0) {
        ToMemory(PC, (addr & 0xFF0000) >> 16);
        ToMemory(PC, (addr & 0xFF00) >> 8);
        ToMemory(PC, addr & 0xFF);
    } else {
        ToMemory(PC, (Val(StringField(dat, r, ",")) & 0xFF0000) >> 16);
        ToMemory(PC, (Val(StringField(dat, r, ",")) & 0xFF00) >> 8);
        ToMemory(PC, Val(StringField(dat, r, ",")) & 0xFF);
    }
}

std::string ApplyCode::CompileLine(std::string s, uint32_t l) {
    // delete comments from line of code
    s = StringField(s, 1, ";");

    if(s == "") { return ""; }

    // separate
    std::string cmd = StringField(s, 1, " ");
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);
    Trim(cmd);

    std::string dat = s.substr(cmd.length(), s.length() - cmd.length());
    std::transform(dat.begin(), dat.end(), dat.begin(), ::toupper);
    Trim(dat);
  
    if(cmd == "") { return ""; }

    if(cmd[cmd.size() - 1] == ':') {        
        for(uint32_t i = 1; i < cmd.size(); i++) {
            std::string a = cmd.substr(i, 1);

            if((a < "A" || a > "Z") && (a != "@") && (a != "_")) {
                if(pass == 1) {
                    statusBarUpdater("Bad label at line " + std::to_string(l));
                }
            }
        }

        if(pass == 1) {
            label[l][0] = cmd.substr(0, cmd.size() - 1);
            label[l][1] = Hex(PC);
        }

        for(uint32_t j = 0; j < l; j++) {
            if(label[j][1] == label[l][1]) {
                if(pass == 1) {
                    statusBarUpdater("Duplicate label at line " + std::to_string(l));
                }
            }
        }

        s = dat;

        cmd = StringField(s, 1, " ");
        std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);
        Trim(cmd);

        dat = s.substr(cmd.length(), s.length() - cmd.length());
        std::transform(dat.begin(), dat.end(), dat.begin(), ::toupper);
        Trim(dat);

        if(cmd == "") { return ""; }
    }

    // write commands in memory
    if(cmd == "NOP") {
        if(CheckGarbages(dat)) {
            return "Garbages found at line " + std::to_string(l);
        } else {
            ToMemory(PC, 0x00);
        }
    } else if(cmd == "HALT") {
        if(CheckGarbages(dat)) {
            return "Garbages found at line " + std::to_string(l);
        } else {
            ToMemory(PC, 0x01);
        }
    } else if(cmd == "JP") {
        if(CountString(dat, ",") != 1 && CountString(dat, ",") != 2) {
            return "Bad parameters at line " + std::to_string(l);
        } else if(CountString(dat, ",") == 2) {
            // test flags
            std::string flag = StringField(dat, 1, ",");

            if(flag == "N") {
                ToMemory(PC, 0x0D); // negative
            } else if(flag == "P") {
                ToMemory(PC, 0x0E); // positive
            } else if(flag == "Z") {
                ToMemory(PC, 0x0F); // zero
            } else if(flag == "NZ") {
                ToMemory(PC, 0x10); // not zero
            } else if(flag == "H") {
                ToMemory(PC, 0x11); // half-carry set
            } else if(flag == "NH") {
                ToMemory(PC, 0x12); // not half-carry set
            } else if(flag == "OV") {
                ToMemory(PC, 0x13); // overflow
            } else if(flag == "NO") {
                ToMemory(PC, 0x14); // no overflow
            } else if(flag == "C") {
                ToMemory(PC, 0x15); // carry set
            } else if(flag == "NC") {
                ToMemory(PC, 0x16); // not carry set
            } else {
                return "Bad parameters at line " + std::to_string(l);
            }

            WriteLabelOrValue(2, dat, l);
        } else if(CountString(dat, ",") == 1) {
            if(dat == "A0") {
                ToMemory(PC, 0x05);
            } else if(dat == "A1") {
                ToMemory(PC, 0x06);
            } else if(dat == "A2") {
                ToMemory(PC, 0x07);
            } else if(dat == "A3") {
                ToMemory(PC, 0x08);
            } else if(dat == "A4") {
                ToMemory(PC, 0x09);
            } else if(dat == "A5") {
                ToMemory(PC, 0x0A);
            } else if(dat == "A6") {
                ToMemory(PC, 0x0B);
            } else if(dat == "A7") {
                ToMemory(PC, 0x0C);
            } else {
                ToMemory(PC, 0x02);
                
                WriteLabelOrValue(1, dat, l);
            }
        }
    } else if(cmd == "CALL") {
        if(CountString(dat, ",") != 1) {
            return "Bad parameters at line " + std::to_string(l);
        } else {
            ToMemory(PC, 0x03);
            
            WriteLabelOrValue(1, dat, l);
        }
    } else if(cmd == "RET") {
        if(CountString(dat, ",") != 1) {
            return "Bad parameters at line " + std::to_string(l);
        } else {
            if(dat == "N") {
                ToMemory(PC, 0x17);
            } else if(dat == "P") {
                ToMemory(PC, 0x18);
            } else if(dat == "Z") {
                ToMemory(PC, 0x19);
            } else if(dat == "NZ") {
                ToMemory(PC, 0x1A);
            } else if(dat == "H") {
                ToMemory(PC, 0x1B);
            } else if(dat == "NH") {
                ToMemory(PC, 0x1C);
            } else if(dat == "OV") {
                ToMemory(PC, 0x1D);
            } else if(dat == "NO") {
                ToMemory(PC, 0x1E);
            } else if(dat == "C") {
                ToMemory(PC, 0x1F);
            } else if(dat == "NC") {
                ToMemory(PC, 0x20);
            } else {
                if(CheckGarbages(dat)) {
                    return "Garbages found at line " + std::to_string(l);
                } else {
                    ToMemory(PC, 0x04);
                }
            }
        }
    } else if(cmd == "SEZ") {
        if(CheckGarbages(dat)) {
            return "Garbages found at line " + std::to_string(l);
        } else {
            ToMemory(PC, 0x21);
        }
    } else if(cmd == "CLZ") {
        if(CheckGarbages(dat)) {
            return "Garbages found at line " + std::to_string(l);
        } else {
            ToMemory(PC, 0x22);
        }
    } else if(cmd == "SEC") {
        if(CheckGarbages(dat)) {
            return "Garbages found at line " + std::to_string(l);
        } else {
            ToMemory(PC, 0x23);
        }
    } else if(cmd == "CLC") {
        if(CheckGarbages(dat)) {
            return "Garbages found at line " + std::to_string(l);
        } else {
            ToMemory(PC, 0x24);
        }
    } else if(cmd == "CP") {
        if(CountString(dat, ",") != 1 && CountString(dat, ",") != 2) {
            return "Bad parameters at line " + std::to_string(l);
        } else if(CountString(dat, ",") == 1) {
            if(dat == "[A0]") {
                ToMemory(PC, 0x25);
            } else if(dat == "D1") {
                ToMemory(PC, 0x26);
            } else {
                ToMemory(PC, 0x27);
                ToMemory(PC, Val(StringField(dat, 1, ",")) & 0xff);
            }
        } else if(CountString(dat, ",") == 2) {
            if(StringField(dat, 1, ",") == "A0") {
                ToMemory(PC, 0xEF);
                
                WriteLabelOrValue(2, dat, l);
            }
        }
    } else if(cmd == "LD") {
        if(CountString(dat, ",") != 2) {
            return "Bad parameters at line " + std::to_string(l);
        } else {
            if(dat == "D0,[A0]") {
                ToMemory(PC, 0x37);
            } else if(dat == "D1,[A1]") {
                ToMemory(PC, 0x38);
            } else if(dat == "D2,[A2]") {
                ToMemory(PC, 0x39);
            } else if(dat == "D3,[A3]") {
                ToMemory(PC, 0x3A);
            } else if(dat == "D4,[A4]") {
                ToMemory(PC, 0x3B);
            } else if(dat == "D5,[A5]") {
                ToMemory(PC, 0x3C);
            } else if(dat == "D6,[A6]") {
                ToMemory(PC, 0x3D);
            } else if(dat == "[A0],D0") {
                ToMemory(PC, 0x3E);
            } else if(dat == "[A1],D1") {
                ToMemory(PC, 0x3F);
            } else if(dat == "[A2],D2") {
                ToMemory(PC, 0x40);
            } else if(dat == "[A3],D3") {
                ToMemory(PC, 0x41);
            } else if(dat == "[A4],D4") {
                ToMemory(PC, 0x42);
            } else if(dat == "[A5],D5") {
                ToMemory(PC, 0x43);
            } else if(dat == "[A6],D6") {
                ToMemory(PC, 0x44);
            } else if(dat == "D1,D0") {
                ToMemory(PC, 0x45);
            } else if(dat == "D2,D0") {
                ToMemory(PC, 0x46);
            } else if(dat == "D3,D0") {
                ToMemory(PC, 0x47);
            } else if(dat == "D4,D0") {
                ToMemory(PC, 0x48);
            } else if(dat == "D5,D0") {
                ToMemory(PC, 0x49);
            } else if(dat == "D6,D0") {
                ToMemory(PC, 0x4A);
            } else if(dat == "D0,D1") {
                ToMemory(PC, 0x4B);
            } else if(dat == "D2,D1") {
                ToMemory(PC, 0x4C);
            } else if(dat == "D3,D1") {
                ToMemory(PC, 0x4D);
            } else if(dat == "D4,D1") {
                ToMemory(PC, 0x4E);
            } else if(dat == "D5,D1") {
                ToMemory(PC, 0x4F);
            } else if(dat == "D6,D1") {
                ToMemory(PC, 0x50);
            } else if(dat == "D0,D2") {
                ToMemory(PC, 0x51);
            } else if(dat == "D1,D2") {
                ToMemory(PC, 0x52);
            } else if(dat == "D3,D2") {
                ToMemory(PC, 0x53);
            } else if(dat == "D4,D2") {
                ToMemory(PC, 0x54);
            } else if(dat == "D5,D2") {
                ToMemory(PC, 0x55);
            } else if(dat == "D6,D2") {
                ToMemory(PC, 0x56);
            } else if(dat == "D0,D3") {
                ToMemory(PC, 0x57);
            } else if(dat == "D1,D3") {
                ToMemory(PC, 0x58);
            } else if(dat == "D2,D3") {
                ToMemory(PC, 0x59);
            } else if(dat == "D4,D3") {
                ToMemory(PC, 0x5A);
            } else if(dat == "D5,D3") {
                ToMemory(PC, 0x5B);
            } else if(dat == "D6,D3") {
                ToMemory(PC, 0x5C);
            } else if(dat == "D0,D4") {
                ToMemory(PC, 0x5D);
            } else if(dat == "D1,D4") {
                ToMemory(PC, 0x5E);
            } else if(dat == "D2,D4") {
                ToMemory(PC, 0x5F);
            } else if(dat == "D3,D4") {
                ToMemory(PC, 0x60);
            } else if(dat == "D5,D4") {
                ToMemory(PC, 0x61);
            } else if(dat == "D6,D4") {
                ToMemory(PC, 0x62);
            } else if(dat == "D0,D5") {
                ToMemory(PC, 0x63);
            } else if(dat == "D1,D5") {
                ToMemory(PC, 0x64);
            } else if(dat == "D2,D5") {
                ToMemory(PC, 0x65);
            } else if(dat == "D3,D5") {
                ToMemory(PC, 0x66);
            } else if(dat == "D4,D5") {
                ToMemory(PC, 0x67);
            } else if(dat == "D6,D5") {
                ToMemory(PC, 0x68);
            } else if(dat == "D0,D6") {
                ToMemory(PC, 0x69);
            } else if(dat == "D1,D6") {
                ToMemory(PC, 0x6A);
            } else if(dat == "D2,D6") {
                ToMemory(PC, 0x6B);
            } else if(dat == "D3,D6") {
                ToMemory(PC, 0x6C);
            } else if(dat == "D4,D6") {
                ToMemory(PC, 0x6D);
            } else if(dat == "D5,D6") {
                ToMemory(PC, 0x6E);
            } else {
                std::string reg = StringField(dat, 1, ",");

                if(reg == "A0") {
                    ToMemory(PC, 0x28);
                                    
                    WriteLabelOrValue(2, dat, l);
                } else if(reg == "A1") {
                    ToMemory(PC, 0x29);
                                    
                    WriteLabelOrValue(2, dat, l);
                } else if(reg == "A2") {
                    ToMemory(PC, 0x2A);
                                    
                    WriteLabelOrValue(2, dat, l);
                } else if(reg == "A3") {
                    ToMemory(PC, 0x2B);
                                    
                    WriteLabelOrValue(2, dat, l);
                } else if(reg == "A4") {
                    ToMemory(PC, 0x2C);
                                    
                    WriteLabelOrValue(2, dat, l);
                } else if(reg == "A5") {
                    ToMemory(PC, 0x2D);
                                    
                    WriteLabelOrValue(2, dat, l);
                } else if(reg == "A6") {
                    ToMemory(PC, 0x2E);
                                    
                    WriteLabelOrValue(2, dat, l);
                } else if(reg == "A7") {
                    ToMemory(PC, 0x2F);
                                    
                    WriteLabelOrValue(2, dat, l);
                } else if(reg == "D0") {
                    ToMemory(PC, 0x30);
                    ToMemory(PC, Val(StringField(dat, 2, ",")) & 0xFF);
                } else if(reg == "D1") {
                    ToMemory(PC, 0x31);
                    ToMemory(PC, Val(StringField(dat, 2, ",")) & 0xFF);
                } else if(reg == "D2") {
                    ToMemory(PC, 0x32);
                    ToMemory(PC, Val(StringField(dat, 2, ",")) & 0xFF);
                } else if(reg == "D3") {
                    ToMemory(PC, 0x33);
                    ToMemory(PC, Val(StringField(dat, 2, ",")) & 0xFF);
                } else if(reg == "D4") {
                    ToMemory(PC, 0x34);
                    ToMemory(PC, Val(StringField(dat, 2, ",")) & 0xFF);
                } else if(reg == "D5") {
                    ToMemory(PC, 0x35);
                    ToMemory(PC, Val(StringField(dat, 2, ",")) & 0xFF);
                } else if(reg == "D6") {
                    ToMemory(PC, 0x36);
                    ToMemory(PC, Val(StringField(dat, 2, ",")) & 0xFF);
                } else {
                    return "Bad parameters at line " + std::to_string(l);
                }
            }
        }
    } else if(cmd == "LDI") {
        if(CheckGarbages(dat)) {
            return "Garbages found at line " + std::to_string(l);
        } else {
            ToMemory(PC, 0x6F);
        }
    } else if(cmd == "LDIR") {
        if(CheckGarbages(dat)) {
            return "Garbages found at line " + std::to_string(l);
        } else {
            ToMemory(PC, 0x70);
        }
    } else if(cmd == "LDD") {
        if(CheckGarbages(dat)) {
            return "Garbages found at line " + std::to_string(l);
        } else {
            ToMemory(PC, 0x71);
        }
    } else if(cmd == "LDDR") {
        if(CheckGarbages(dat)) {
            return "Garbages found at line " + std::to_string(l);
        } else {
            ToMemory(PC, 0x72);
        }
    } else if(cmd == "ADD") {
        if(CountString(dat, ",") != 1) {
            return "Bad parameters at line " + std::to_string(l);
        } else {
            if(dat == "D1") {
                ToMemory(PC, 0x73);
            } else if(dat == "D2") {
                ToMemory(PC, 0x74);
            } else if(dat == "D3") {
                ToMemory(PC, 0x75);
            } else if(dat == "D4") {
                ToMemory(PC, 0x76);
            } else if(dat == "D5") {
                ToMemory(PC, 0x77);
            } else if(dat == "D6") {
                ToMemory(PC, 0x78);
            } else {
                return "Bad parameters at line " + std::to_string(l);
            }
        }
    } else if(cmd == "SUB") {
        if(CountString(dat, ",") != 1) {
            return "Bad parameters at line " + std::to_string(l);
        } else {
            if(dat == "D1") {
                ToMemory(PC, 0x79);
            } else if(dat == "D2") {
                ToMemory(PC, 0x7A);
            } else if(dat == "D3") {
                ToMemory(PC, 0x7B);
            } else if(dat == "D4") {
                ToMemory(PC, 0x7C);
            } else if(dat == "D5") {
                ToMemory(PC, 0x7D);
            } else if(dat == "D6") {
                ToMemory(PC, 0x7E);
            } else {
                return "Bad parameters at line " + std::to_string(l);
            }
        }
    } else if(cmd == "MULT") {
        if(CountString(dat, ",") != 2) {
            return "Bad parameters at line " + std::to_string(l);
        } else {
            if(dat == "D0,D1") {
                ToMemory(PC, 0x7F);
            } else if(dat == "D0,D2") {
                ToMemory(PC, 0x80);
            } else if(dat == "D0,D3") {
                ToMemory(PC, 0x81);
            } else if(dat == "D0,D4") {
                ToMemory(PC, 0x82);
            } else if(dat == "D0,D5") {
                ToMemory(PC, 0x83);
            } else if(dat == "D0,D6") {
                ToMemory(PC, 0x84);
            } else if(dat == "A0,A1") {
                ToMemory(PC, 0x85);
            } else if(dat == "A0,D0") {
                ToMemory(PC, 0x86);
            } else {
                return "Bad parameters at line " + std::to_string(l);
            }
        }
    } else if(cmd == "DIV") {
        if(CountString(dat, ",") != 2) {
            return "Bad parameters at line " + std::to_string(l);
        } else {
            if(dat == "A0,A1") {
                ToMemory(PC, 0x87);
            } else if(dat == "A0,D0") {
                ToMemory(PC, 0x88);
            } else {
                return "Bad parameters at line " + std::to_string(l);
            }
        }
    } else if(cmd == "AND") {
        if(CountString(dat, ",") != 1) {
            return "Bad parameters at line " + std::to_string(l);
        } else {
            if(dat == "D1") {
                ToMemory(PC, 0x89);
            } else if(dat == "D2") {
                ToMemory(PC, 0x8A);
            } else if(dat == "D3") {
                ToMemory(PC, 0x8B);
            } else if(dat == "D4") {
                ToMemory(PC, 0x8C);
            } else if(dat == "D5") {
                ToMemory(PC, 0x8D);
            } else if(dat == "D6") {
                ToMemory(PC, 0x8E);
            } else {
                return "Bad parameters at line " + std::to_string(l);
            }
        }
    } else if(cmd == "OR") {
        if(CountString(dat, ",") != 1) {
            return "Bad parameters at line " + std::to_string(l);
        } else {
            if(dat == "D1") {
                ToMemory(PC, 0x8F);
            } else if(dat == "D2") {
                ToMemory(PC, 0x90);
            } else if(dat == "D3") {
                ToMemory(PC, 0x91);
            } else if(dat == "D4") {
                ToMemory(PC, 0x92);
            } else if(dat == "D5") {
                ToMemory(PC, 0x93);
            } else if(dat == "D6") {
                ToMemory(PC, 0x94);
            } else {
                return "Bad parameters at line " + std::to_string(l);
            }
        }
    } else if(cmd == "XOR") {
        if(CountString(dat, ",") != 1) {
            return "Bad parameters at line " + std::to_string(l);
        } else {
            if(dat == "D0") {
                ToMemory(PC, 0x9B);
            } else if(dat == "D1") {
                ToMemory(PC, 0x95);
            } else if(dat == "D2") {
                ToMemory(PC, 0x96);
            } else if(dat == "D3") {
                ToMemory(PC, 0x97);
            } else if(dat == "D4") {
                ToMemory(PC, 0x98);
            } else if(dat == "D5") {
                ToMemory(PC, 0x99);
            } else if(dat == "D6") {
                ToMemory(PC, 0x9A);
            } else {
                return "Bad parameters at line " + std::to_string(l);
            }
        }
    } else if(cmd == "SET") {
        if(CountString(dat, ",") != 2) {
            return "Bad parameters at line " + std::to_string(l);
        } else {
            if(dat == "D0,0") {
                ToMemory(PC, 0x9C);
            } else if(dat == "D0,1") {
                ToMemory(PC, 0x9D);
            } else if(dat == "D0,2") {
                ToMemory(PC, 0x9E);
            } else if(dat == "D0,3") {
                ToMemory(PC, 0x9F);
            } else if(dat == "D0,4") {
                ToMemory(PC, 0xA0);
            } else if(dat == "D0,5") {
                ToMemory(PC, 0xA1);
            } else if(dat == "D0,6") {
                ToMemory(PC, 0xA2);
            } else if(dat == "D0,7") {
                ToMemory(PC, 0xA3);
            } else {
                return "Bad parameters at line " + std::to_string(l);
            }
        }
    } else if(cmd == "RESET") {
        if(CountString(dat, ",") != 2) {
            return "Bad parameters at line " + std::to_string(l);
        } else {
            if(dat == "D0,0") {
                ToMemory(PC, 0xA4);
            } else if(dat == "D0,1") {
                ToMemory(PC, 0xA5);
            } else if(dat == "D0,2") {
                ToMemory(PC, 0xA6);
            } else if(dat == "D0,3") {
                ToMemory(PC, 0xA7);
            } else if(dat == "D0,4") {
                ToMemory(PC, 0xA8);
            } else if(dat == "D0,5") {
                ToMemory(PC, 0xA9);
            } else if(dat == "D0,6") {
                ToMemory(PC, 0xAA);
            } else if(dat == "D0,7") {
                ToMemory(PC, 0xAB);
            } else {
                return "Bad parameters at line " + std::to_string(l);
            }
        }
    } else if(cmd == "BIT") {
        if(CountString(dat, ",") != 2) {
            return "Bad parameters at line " + std::to_string(l);
        } else {
            if(dat == "D0,0") {
                ToMemory(PC, 0xAC);
            } else if(dat == "D0,1") {
                ToMemory(PC, 0xAD);
            } else if(dat == "D0,2") {
                ToMemory(PC, 0xAE);
            } else if(dat == "D0,3") {
                ToMemory(PC, 0xAF);
            } else if(dat == "D0,4") {
                ToMemory(PC, 0xB0);
            } else if(dat == "D0,5") {
                ToMemory(PC, 0xB1);
            } else if(dat == "D0,6") {
                ToMemory(PC, 0xB2);
            } else if(dat == "D0,7") {
                ToMemory(PC, 0xB3);
            } else {
                return "Bad parameters at line " + std::to_string(l);
            }
        }
    } else if(cmd == "PUSH") {
        if((CountString(dat, ",") != 1) && (CountString(dat, ",") != 3)) {
            return "Bad parameters at line " + std::to_string(l);
        } else if(CountString(dat, ",") == 1) {
            if(dat == "A0") {
                ToMemory(PC, 0xB4);
            } else if(dat == "A1") {
                ToMemory(PC, 0xB5);
            } else if(dat == "A2") {
                ToMemory(PC, 0xB6);
            } else if(dat == "A3") {
                ToMemory(PC, 0xB7);
            } else if(dat == "A4") {
                ToMemory(PC, 0xB8);
            } else if(dat == "A5") {
                ToMemory(PC, 0xB9);
            } else if(dat == "A6") {
                ToMemory(PC, 0xBA);
            } else if(dat == "A7") {
                ToMemory(PC, 0xBB);
            } else {
                return "Bad parameters at line " + std::to_string(l);                
            }
        } else if(CountString(dat, ",") == 3) {
            if(dat == "D0,D1,D2") {
                ToMemory(PC, 0xBC);
            } else if(dat == "D1,D2,D3") {
                ToMemory(PC, 0xBD);
            } else if(dat == "D2,D3,D4") {
                ToMemory(PC, 0xBE);
            } else if(dat == "D3,D4,D5") {
                ToMemory(PC, 0xBF);
            } else {
                return "Bad parameters at line " + std::to_string(l);                
            }
        }
    } else if(cmd == "POP") {
        if((CountString(dat, ",") != 1) && (CountString(dat, ",") != 3)) {
            return "Bad parameters at line " + std::to_string(l);
        } else if(CountString(dat, ",") == 1) {
            if(dat == "A0") {
                ToMemory(PC, 0xC0);
            } else if(dat == "A1") {
                ToMemory(PC, 0xC1);
            } else if(dat == "A2") {
                ToMemory(PC, 0xC2);
            } else if(dat == "A3") {
                ToMemory(PC, 0xC3);
            } else if(dat == "A4") {
                ToMemory(PC, 0xC4);
            } else if(dat == "A5") {
                ToMemory(PC, 0xC5);
            } else if(dat == "A6") {
                ToMemory(PC, 0xC6);
            } else if(dat == "A7") {
                ToMemory(PC, 0xC7);
            } else {
                return "Bad parameters at line " + std::to_string(l);                
            }
        } else if(CountString(dat, ",") == 3) {
            if(dat == "D0,D1,D2") {
                ToMemory(PC, 0xC8);
            } else if(dat == "D1,D2,D3") {
                ToMemory(PC, 0xC9);
            } else if(dat == "D2,D3,D4") {
                ToMemory(PC, 0xCA);
            } else if(dat == "D3,D4,D5") {
                ToMemory(PC, 0xCB);
            } else {
                return "Bad parameters at line " + std::to_string(l);                
            }
        }
    } else if(cmd == "SHL") {
        if(CountString(dat, ",") != 1) {
            return "Bad parameters at line " + std::to_string(l);
        } else {
            if(dat == "D0") {
                ToMemory(PC, 0xCC);
            } else {
                return "Bad parameters at line " + std::to_string(l);                
            }
        }
    } else if(cmd == "SHR") {
        if(CountString(dat, ",") != 1) {
            return "Bad parameters at line " + std::to_string(l);
        } else {
            if(dat == "D0") {
                ToMemory(PC, 0xCD);
            } else {
                return "Bad parameters at line " + std::to_string(l);                
            }
        }
    } else if(cmd == "RL") {
        if(CountString(dat, ",") != 1) {
            return "Bad parameters at line " + std::to_string(l);
        } else {
            if(dat == "D0") {
                ToMemory(PC, 0xCE);
            } else {
                return "Bad parameters at line " + std::to_string(l);                
            }
        }
    } else if(cmd == "RR") {
        if(CountString(dat, ",") != 1) {
            return "Bad parameters at line " + std::to_string(l);
        } else {
            if(dat == "D0") {
                ToMemory(PC, 0xCF);
            } else {
                return "Bad parameters at line " + std::to_string(l);                
            }
        }
    } else if(cmd == "INC") {
        if(CountString(dat, ",") != 1) {
            return "Bad parameters at line " + std::to_string(l);
        } else {
            if(dat == "D0") {
                ToMemory(PC, 0xD0);
            } else if(dat == "D1") {
                ToMemory(PC, 0xD1);
            } else if(dat == "D2") {
                ToMemory(PC, 0xD2);
            } else if(dat == "D3") {
                ToMemory(PC, 0xD3);
            } else if(dat == "D4") {
                ToMemory(PC, 0xD4);
            } else if(dat == "D5") {
                ToMemory(PC, 0xD5);
            } else if(dat == "D6") {
                ToMemory(PC, 0xD6);
            } else if(dat == "A0") {
                ToMemory(PC, 0xDF);
            } else if(dat == "A1") {
                ToMemory(PC, 0xE0);
            } else if(dat == "A2") {
                ToMemory(PC, 0xE1);
            } else if(dat == "A3") {
                ToMemory(PC, 0xE2);
            } else if(dat == "A4") {
                ToMemory(PC, 0xE3);
            } else if(dat == "A5") {
                ToMemory(PC, 0xE4);
            } else if(dat == "A6") {
                ToMemory(PC, 0xE5);
            } else if(dat == "A7") {
                ToMemory(PC, 0xE6);
            } else {
                return "Bad parameters at line " + std::to_string(l);                
            }
        }
    } else if(cmd == "DEC") {
        if(CountString(dat, ",") != 1) {
            return "Bad parameters at line " + std::to_string(l);
        } else {
            if(dat == "D0") {
                ToMemory(PC, 0xD7);
            } else if(dat == "D1") {
                ToMemory(PC, 0xD8);
            } else if(dat == "D2") {
                ToMemory(PC, 0xD9);
            } else if(dat == "D3") {
                ToMemory(PC, 0xDA);
            } else if(dat == "D4") {
                ToMemory(PC, 0xDB);
            } else if(dat == "D5") {
                ToMemory(PC, 0xDC);
            } else if(dat == "D6") {
                ToMemory(PC, 0xDD);
            } else if(dat == "A0") {
                ToMemory(PC, 0xE7);
            } else if(dat == "A1") {
                ToMemory(PC, 0xE8);
            } else if(dat == "A2") {
                ToMemory(PC, 0xE9);
            } else if(dat == "A3") {
                ToMemory(PC, 0xEA);
            } else if(dat == "A4") {
                ToMemory(PC, 0xEB);
            } else if(dat == "A5") {
                ToMemory(PC, 0xEC);
            } else if(dat == "A6") {
                ToMemory(PC, 0xED);
            } else if(dat == "A7") {
                ToMemory(PC, 0xEE);
            } else {
                return "Bad parameters at line " + std::to_string(l);                
            }
        }
    } else if(cmd == "CLS") {
        if(CheckGarbages(dat)) {
            return "Garbages found at line " + std::to_string(l);
        } else {
            ToMemory(PC, 0xFF);
            ToMemory(PC, 0x00);
        }
    } else if(cmd == "SPRITE") {
        if(CheckGarbages(dat)) {
            return "Garbages found at line " + std::to_string(l);
        } else {
            ToMemory(PC, 0xFF);
            ToMemory(PC, 0x01);
        }
    } else if(cmd == "BLOCK") {
        if(CheckGarbages(dat)) {
            return "Garbages found at line " + std::to_string(l);
        } else {
            ToMemory(PC, 0xFF);
            ToMemory(PC, 0x02);
        }
    } else if(cmd == "GRAB") {
        if(CheckGarbages(dat)) {
            return "Garbages found at line " + std::to_string(l);
        } else {
            ToMemory(PC, 0xFF);
            ToMemory(PC, 0x03);
        }
    } else if(cmd == "GETJOYSTICK") {
        if(CheckGarbages(dat)) {
            return "Garbages found at line " + std::to_string(l);
        } else {
            ToMemory(PC, 0xFF);
            ToMemory(PC, 0x04);
        }
    } else if(cmd == "LOCATE") {
        if(CountString(dat, ",") != 2) {
            return "Bad parameters at line " + std::to_string(l);
        } else {
            ToMemory(PC, 0xFF);
            ToMemory(PC, 0x05);
            ToMemory(PC, Val(StringField(dat, 1, ",")));
            ToMemory(PC, Val(StringField(dat, 2, ",")));
        }
    } else if(cmd == "PRINTCHAR") {
        if(CheckGarbages(dat)) {
            return "Garbages found at line " + std::to_string(l);
        } else {
            ToMemory(PC, 0xFF);
            ToMemory(PC, 0x06);
        }
    } else if(cmd == "PRINT") {
        if(CountString(dat, ",") != 1) {
            return "Bad parameters at line " + std::to_string(l);
        } else {
            ToMemory(PC, 0xFF);
            ToMemory(PC, 0x07);
    
            WriteLabelOrValue(1, dat, l);
        }
    } else if(cmd == "PEN") {
        if(CountString(dat, ",") != 1) {
            return "Bad parameters at line " + std::to_string(l);
        } else {
            ToMemory(PC, 0xFF);
            ToMemory(PC, 0x08);
            ToMemory(PC, Val(StringField(dat, 1, ",")) & 0xFF);
        }
    } else if(cmd == "PAPER") {
        if(CountString(dat, ",") != 1) {
            return "Bad parameters at line " + std::to_string(l);
        } else {
            ToMemory(PC, 0xFF);
            ToMemory(PC, 0x09);
            ToMemory(PC, Val(StringField(dat, 1, ",")) & 0xFF);
        }
    } else if(cmd == "BORDER") {
        if(CountString(dat, ",") != 1) {
            return "Bad parameters at line " + std::to_string(l);
        } else {
            ToMemory(PC, 0xFF);
            ToMemory(PC, 0x0A);
            ToMemory(PC, Val(StringField(dat, 1, ",")) & 0xFF);
        }
    } else if(cmd == "PLOT") {
        if(CountString(dat, ",") != 3) {
            return "Bad parameters at line " + std::to_string(l);
        } else {
            if(dat == "A0,A1,D0") {
                ToMemory(PC, 0xFF);
                ToMemory(PC, 0x0B);
            }
        }
    } else if(cmd == "POINT") {
        if(CountString(dat, ",") != 2) {
            return "Bad parameters at line " + std::to_string(l);
        } else {
            if(dat == "A0,A1") {
                ToMemory(PC, 0xFF);
                ToMemory(PC, 0x0C);
            }
        }
    } else if(cmd == "SCROLL") {
        if(CountString(dat, ",") != 2) {
            return "Bad parameters at line " + std::to_string(l);
        } else {
            ToMemory(PC, 0xFF);
            ToMemory(PC, 0x0D);
            ToMemory(PC, Val(StringField(dat, 1, ",")) & 0xFF);
            ToMemory(PC, Val(StringField(dat, 2, ",")) & 0xFF);
        }
    } else if(cmd == "FLIP") {
        if(CheckGarbages(dat)) {
            return "Garbages found at line " + std::to_string(l);
        } else {
            ToMemory(PC, 0xFF);
            ToMemory(PC, 0x0E);
        }
    } else if(cmd == "SOUND") {
        if(CountString(dat, ",") != 2) {
            return "Bad parameters at line " + std::to_string(l);
        } else {
            ToMemory(PC, 0xFF);
            ToMemory(PC, 0x0F);
            ToMemory(PC, (Val(StringField(dat, 1, ",")) & 0xFF0000) >> 16);
            ToMemory(PC, (Val(StringField(dat, 1, ",")) & 0xFF00) >> 8);
            ToMemory(PC, Val(StringField(dat, 1, ",")) & 0xFF);
            ToMemory(PC, (Val(StringField(dat, 2, ",")) & 0xFF0000) >> 16);
            ToMemory(PC, (Val(StringField(dat, 2, ",")) & 0xFF00) >> 8);
            ToMemory(PC, Val(StringField(dat, 2, ",")) & 0xFF);
        }
    } else if(cmd == "MUSIC") {
        if(CountString(dat, ",") != 2) {
            return "Bad parameters at line " + std::to_string(l);
        } else {
            ToMemory(PC, 0xFF);
            ToMemory(PC, 0x10);
            ToMemory(PC, (Val(StringField(dat, 1, ",")) & 0xFF0000) >> 16);
            ToMemory(PC, (Val(StringField(dat, 1, ",")) & 0xFF00) >> 8);
            ToMemory(PC, Val(StringField(dat, 1, ",")) & 0xFF);
            ToMemory(PC, (Val(StringField(dat, 2, ",")) & 0xFF0000) >> 16);
            ToMemory(PC, (Val(StringField(dat, 2, ",")) & 0xFF00) >> 8);
            ToMemory(PC, Val(StringField(dat, 2, ",")) & 0xFF);
        }
    } else if(cmd == "STOP") {
        if(CheckGarbages(dat)) {
            return "Garbages found at line " + std::to_string(l);
        } else {
            ToMemory(PC, 0xFF);
            ToMemory(PC, 0x11);
        }
    } else if(cmd == "MAP") {
        if(CheckGarbages(dat)) {
            return "Garbages found at line " + std::to_string(l);
        } else {
            ToMemory(PC, 0xFF);
            ToMemory(PC, 0x12);
        }
    } else if(cmd == "DB") {
        if(dat == "") {
            return "Bad parameters at line " + std::to_string(l);
        } else {
            for(uint32_t i = 1; i <= CountString(dat, ","); i++) {
                std::string d = StringField(dat, i, ",");

                if(d[0] == '"' && d[d.length() - 1] == '"') {
                    d.erase(0, 1); // remove first char
                    d.pop_back(); // remove last char

                    for(uint32_t j = 0; j < d.size(); j++) {
                        if(d[j] == '"') {
                            return "Bad string at line " + std::to_string(l);
                        }
                    }

                    for(uint32_t j = 0; j < d.size(); j++) {
                        ToMemory(PC, (char)d[j]);
                    }
                } else {
                    char dt = Val(StringField(dat, 1, ",")) & 0xFF;
                    ToMemory(PC, dt);
                }
            }
        }
    } else if(cmd == "DS") {
        if(CountString(dat, ",") != 2) {
            return "Bad parameters at line " + std::to_string(l);
        } else {
            uint32_t ln = Val(StringField(dat, 1, ",")) & 0xFFFFFF;

            if(ln > 0) {
                for(uint32_t i = 0; i < ln; i++) {
                    ToMemory(PC, (Val(StringField(dat, 2, ",")) & 0xFF0000) >> 16);
                    ToMemory(PC, (Val(StringField(dat, 2, ",")) & 0xFF00) >> 8);
                    ToMemory(PC, (Val(StringField(dat, 2, ",")) & 0xFF));
                }
            } else {
                return "DS length error at line " + std::to_string(l);
            }
        }
    } else {
      return "Syntax error at line "+ std::to_string(l);
    }

    return "";
}

void ApplyCode::setStatusBarUpdater(const std::function<void(const std::string&)>& updater) {
    statusBarUpdater = updater;
}

void ApplyCode::updateStatus(const std::string& msg) {
    statusBarUpdater(msg); // sûr, jamais vide
}