#ifndef LEVEL_EDITOR_HPP
#define LEVEL_EDITOR_HPP

#include <gtk/gtk.h>
#include <vector>
#include <cstdint>

// Drawing mode enum
enum DrawMode {
    DRAW_MODE,
    FILL_MODE
};

// Structure for level editor data
struct LevelEditorData {
    GtkWidget *tileset_combo;
    GtkWidget *tileset_image;
    GtkWidget *selected_tile_image;
    GtkWidget *map_drawing_area;
    GtkWidget *width_spinbutton;
    GtkWidget *height_spinbutton;
    GtkWidget *draw_button;
    GtkWidget *fill_button;
    
    int selected_tileset;
    int selected_tile_x;
    int selected_tile_y;
    int selected_tile_index;
    
    int map_width;  // en tiles
    int map_height; // en tiles
    
    DrawMode current_mode; // Current drawing mode
    
    std::vector<std::vector<int>> map_data; // map_data[y][x] = tile_index
};

// Open the level editor window
void open_level_editor(GtkWindow *parent_window, GtkWidget *message_label);

#endif // LEVEL_EDITOR_HPP