CXX := g++
CXXFLAGS := -std=c++17 -g
PKG_CFLAGS := $(shell pkg-config --cflags gtk+-3.0 gtksourceview-3.0 harfbuzz)
PKG_LIBS := $(shell pkg-config --libs gtk+-3.0 gtksourceview-3.0 harfbuzz)

TARGET := Monster8Editor
SOURCES := main.cpp code.cpp functions.cpp gtk.cpp

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(PKG_CFLAGS) $^ -o $@ $(PKG_LIBS)

clean:
	rm -f $(TARGET)

.PHONY: all clean