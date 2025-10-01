#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

// Standard C++ includes first
#include <algorithm>
#include <cctype>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

// GTK/GDK includes in correct order
#include <glib.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

// Function declarations
uint32_t get_pixel(GdkPixbuf *pixbuf, int x, int y);
uint32_t CopyImageToRaw(GdkPixbuf *pixbuf, uint32_t addr, uint32_t xframe, uint32_t yframe, uint32_t width, uint32_t height);
GdkPixbuf* ConvertImageTo24bits(uint32_t address, uint32_t width, uint32_t height);

std::vector<std::string> Explode(const std::string &input, const std::string &delim);
std::string StringField(const std::string &str, int n, const std::string &delim);
uint32_t CountString(const std::string &str, const std::string &delim);

void LTrim(std::string &s);
void RTrim(std::string &s);
void Trim(std::string &s);

template <typename T>
std::string Hex(T value) {
    std::ostringstream oss;
    oss << "0x" << std::uppercase << std::hex << value;
    return oss.str();
}

bool StringToInt(const std::string &str, int &outValue);
int Val(std::string s);

#endif
