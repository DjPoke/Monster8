#include "level_editor.hpp"
#include "code.hpp"
#include "functions.hpp"
#include <cmath>
#include <algorithm>
#include <string>

extern ApplyCode applyCode;

// Update tileset image display (256x256)
static void update_tileset_image(LevelEditorData *data) {
    if (data->selected_tileset < 0 || 
        static_cast<uint32_t>(data->selected_tileset) >= applyCode.tilesetCount) {
        return;
    }
    
    uint32_t tileset_idx = static_cast<uint32_t>(data->selected_tileset);
    if (tileset_idx >= applyCode.tilesetAddress.size() ||
        tileset_idx >= applyCode.tilesetWidth.size() ||
        tileset_idx >= applyCode.tilesetHeight.size() ||
        tileset_idx >= applyCode.tileCount.size() ||
        tileset_idx >= applyCode.tileAddress.size() ||
        tileset_idx >= applyCode.tileWidth.size() ||
        tileset_idx >= applyCode.tileHeight.size()) {
        return;
    }
    
    uint32_t width = applyCode.tilesetWidth[tileset_idx];
    uint32_t height = applyCode.tilesetHeight[tileset_idx];
    uint32_t tile_count = applyCode.tileCount[tileset_idx];
    
    if (width == 0 || height == 0 || tile_count == 0) {
        return;
    }
    
    // Get first tile dimensions (assuming all tiles have same size)
    if (applyCode.tileWidth[tileset_idx].size() == 0 ||
        applyCode.tileHeight[tileset_idx].size() == 0) {
        return;
    }
    
    uint32_t tile_width = applyCode.tileWidth[tileset_idx][0];
    uint32_t tile_height = applyCode.tileHeight[tileset_idx][0];
    
    if (tile_width == 0 || tile_height == 0) {
        return;
    }
    
    // Create a surface to draw all tiles
    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, width, height);
    cairo_t *cr = cairo_create(surface);
    
    // Fill background with dark gray
    cairo_set_source_rgb(cr, 0.2, 0.2, 0.2);
    cairo_paint(cr);
    
    // Calculate tiles per row
    int tiles_per_row = width / tile_width;
    
    // Draw each tile
    for (uint32_t i = 0; i < tile_count; i++) {
        if (i >= applyCode.tileAddress[tileset_idx].size() ||
            i >= applyCode.tileWidth[tileset_idx].size() ||
            i >= applyCode.tileHeight[tileset_idx].size()) {
            break;
        }
        
        uint32_t tile_address = applyCode.tileAddress[tileset_idx][i];
        uint32_t t_width = applyCode.tileWidth[tileset_idx][i];
        uint32_t t_height = applyCode.tileHeight[tileset_idx][i];
        
        if (t_width == 0 || t_height == 0) {
            continue;
        }
        
        // Get tile pixbuf
        GdkPixbuf *tile_pixbuf = ConvertImageTo24bits(tile_address + 0x2000000, t_width, t_height);
        if (tile_pixbuf == NULL) {
            continue;
        }
        
        // Calculate position in tileset
        int tile_x = (i % tiles_per_row) * tile_width;
        int tile_y = (i / tiles_per_row) * tile_height;
        
        // Draw tile onto surface
        gdk_cairo_set_source_pixbuf(cr, tile_pixbuf, tile_x, tile_y);
        cairo_paint(cr);
        
        g_object_unref(tile_pixbuf);
    }
    
    cairo_destroy(cr);
    
    // Convert surface to pixbuf
    GdkPixbuf *tileset_pixbuf = gdk_pixbuf_get_from_surface(surface, 0, 0, width, height);
    cairo_surface_destroy(surface);
    
    if (tileset_pixbuf == NULL) {
        return;
    }
    
    // Scale to 256x256 if needed
    GdkPixbuf *scaled_pixbuf = NULL;
    if (width != 256 || height != 256) {
        scaled_pixbuf = gdk_pixbuf_scale_simple(tileset_pixbuf, 256, 256, GDK_INTERP_NEAREST);
        g_object_unref(tileset_pixbuf);
        tileset_pixbuf = scaled_pixbuf;
    }
    
    gtk_image_set_from_pixbuf(GTK_IMAGE(data->tileset_image), tileset_pixbuf);
    g_object_unref(tileset_pixbuf);
}

// Update selected tile preview
static void update_selected_tile_image(LevelEditorData *data) {
    if (data->selected_tileset < 0 || data->selected_tile_index < 0) {
        gtk_image_clear(GTK_IMAGE(data->selected_tile_image));
        return;
    }
    
    uint32_t tileset_idx = static_cast<uint32_t>(data->selected_tileset);
    if (tileset_idx >= applyCode.tileAddress.size()) {
        return;
    }
    
    uint32_t tile_idx = static_cast<uint32_t>(data->selected_tile_index);
    if (tile_idx >= applyCode.tileAddress[tileset_idx].size() ||
        tile_idx >= applyCode.tileWidth[tileset_idx].size() ||
        tile_idx >= applyCode.tileHeight[tileset_idx].size()) {
        return;
    }
    
    uint32_t tile_address = applyCode.tileAddress[tileset_idx][tile_idx];
    uint32_t tile_width = applyCode.tileWidth[tileset_idx][tile_idx];
    uint32_t tile_height = applyCode.tileHeight[tileset_idx][tile_idx];
    
    if (tile_width == 0 || tile_height == 0) {
        return;
    }
    
    GdkPixbuf *tile_pixbuf = ConvertImageTo24bits(tile_address + 0x2000000, tile_width, tile_height);
    if (tile_pixbuf == NULL) {
        return;
    }
    
    // Scale tile to 64x64 for preview
    GdkPixbuf *scaled_pixbuf = gdk_pixbuf_scale_simple(tile_pixbuf, 64, 64, GDK_INTERP_NEAREST);
    g_object_unref(tile_pixbuf);
    
    gtk_image_set_from_pixbuf(GTK_IMAGE(data->selected_tile_image), scaled_pixbuf);
    g_object_unref(scaled_pixbuf);
}

// Callback: tileset combo changed
static void on_tileset_combo_changed(GtkComboBox *combo, gpointer user_data) {
    LevelEditorData *data = static_cast<LevelEditorData*>(user_data);
    data->selected_tileset = gtk_combo_box_get_active(combo);
    data->selected_tile_index = -1;
    
    update_tileset_image(data);
    gtk_image_clear(GTK_IMAGE(data->selected_tile_image));
}

// Callback: click on tileset image to select tile
static gboolean on_tileset_image_clicked(GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
    LevelEditorData *data = static_cast<LevelEditorData*>(user_data);
    
    if (data->selected_tileset < 0 || 
        static_cast<uint32_t>(data->selected_tileset) >= applyCode.tilesetCount) {
        return FALSE;
    }
    
    uint32_t tileset_idx = static_cast<uint32_t>(data->selected_tileset);
    if (tileset_idx >= applyCode.tilesetWidth.size() ||
        tileset_idx >= applyCode.tilesetHeight.size() ||
        tileset_idx >= applyCode.tileCount.size() ||
        tileset_idx >= applyCode.tileWidth.size() ||
        tileset_idx >= applyCode.tileHeight.size()) {
        return FALSE;
    }
    
    // Get first tile dimensions (assuming all tiles have same size)
    if (applyCode.tileWidth[tileset_idx].size() == 0 ||
        applyCode.tileHeight[tileset_idx].size() == 0) {
        return FALSE;
    }
    
    uint32_t tile_width = applyCode.tileWidth[tileset_idx][0];
    uint32_t tile_height = applyCode.tileHeight[tileset_idx][0];
    
    if (tile_width == 0 || tile_height == 0) {
        return FALSE;
    }

    // Calculate number of tiles in each direction using floor
    uint32_t tileset_width = applyCode.tilesetWidth[tileset_idx];
    uint32_t tileset_height = applyCode.tilesetHeight[tileset_idx];
    int tiles_per_row = static_cast<int>(std::floor(static_cast<double>(tileset_width) / tile_width));
    int tiles_per_col = static_cast<int>(std::floor(static_cast<double>(tileset_height) / tile_height));
            
    if (tiles_per_row == 0 || tiles_per_col == 0) {
        return FALSE;
    }

    // Image is displayed as 256x256
    const double DISPLAY_SIZE = 256.0;
    
    // Calculate tile size in displayed image (256x256) using floor
    double display_tile_width = std::floor(DISPLAY_SIZE / tiles_per_row);
    double display_tile_height = std::floor(DISPLAY_SIZE / tiles_per_col);
    
    // Get mouse position: widget is the eventbox, convert to image widget coordinates
    gint img_x, img_y;
    if (!gtk_widget_translate_coordinates(widget, data->tileset_image, 
                                          static_cast<gint>(event->x), 
                                          static_cast<gint>(event->y), 
                                          &img_x, &img_y)) {
        std::cout << "Failed to translate coordinates" << std::endl;
        return FALSE;
    }
    
    // Now get the pixbuf offset within the image widget (image may be centered)
    GtkAllocation img_allocation;
    gtk_widget_get_allocation(data->tileset_image, &img_allocation);
    
    GdkPixbuf *pixbuf = gtk_image_get_pixbuf(GTK_IMAGE(data->tileset_image));
    if (pixbuf == NULL) {
        return FALSE;
    }
    
    int pixbuf_width = gdk_pixbuf_get_width(pixbuf);
    int pixbuf_height = gdk_pixbuf_get_height(pixbuf);
    
    // Calculate offset if image is centered in its allocation
    int offset_x = (img_allocation.width - pixbuf_width) / 2;
    int offset_y = (img_allocation.height - pixbuf_height) / 2;
    
    // Apply offset to get coordinates relative to actual pixbuf
    double mouse_x = static_cast<double>(img_x) - offset_x;
    double mouse_y = static_cast<double>(img_y) - offset_y;
    
    // Check if click is within the image bounds
    if (mouse_x < 0 || mouse_x >= DISPLAY_SIZE || 
        mouse_y < 0 || mouse_y >= DISPLAY_SIZE) {
        return FALSE;
    }
    
    // Calculate which tile was clicked using floor
    int tile_x = static_cast<int>(std::floor(mouse_x / display_tile_width));
    int tile_y = static_cast<int>(std::floor(mouse_y / display_tile_height));
    
    data->selected_tile_x = tile_x;
    data->selected_tile_y = tile_y;
    data->selected_tile_index = tile_y * tiles_per_row + tile_x;
    
    // Check if tile index is valid
    if (data->selected_tile_index >= static_cast<int>(applyCode.tileCount[tileset_idx])) {
        data->selected_tile_index = -1;
        return FALSE;
    }
    
    update_selected_tile_image(data);
    
    return TRUE;
}

// Initialize map data
static void init_map_data(LevelEditorData *data) {
    data->map_data.clear();
    data->map_data.resize(data->map_height);
    for (int y = 0; y < data->map_height; y++) {
        data->map_data[y].resize(data->map_width, 0); // 0 = tile 0
    }
}

// Callback: map size changed
static void on_map_size_changed(GtkSpinButton *spinbutton, gpointer user_data) {
    (void)spinbutton;
    LevelEditorData *data = static_cast<LevelEditorData*>(user_data);
    
    data->map_width = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(data->width_spinbutton));
    data->map_height = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(data->height_spinbutton));
    
    init_map_data(data);
    
    // Update drawing area size to trigger scrollbars when needed
    const int TILE_DISPLAY_SIZE = 32;
    gtk_widget_set_size_request(data->map_drawing_area, 
                                data->map_width * TILE_DISPLAY_SIZE, 
                                data->map_height * TILE_DISPLAY_SIZE);
    
    gtk_widget_queue_draw(data->map_drawing_area);
}

// Draw the map
static gboolean on_map_draw(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
    LevelEditorData *data = static_cast<LevelEditorData*>(user_data);
    
    const int TILE_DISPLAY_SIZE = 32; // Size of each tile on map display
    
    // Draw background
    cairo_set_source_rgb(cr, 0.2, 0.2, 0.2);
    cairo_paint(cr);
    
    // Draw grid and tiles
    for (int y = 0; y < data->map_height; y++) {
        for (int x = 0; x < data->map_width; x++) {
            int px = x * TILE_DISPLAY_SIZE;
            int py = y * TILE_DISPLAY_SIZE;
            
            // Draw tile if set
            int tile_idx = data->map_data[y][x];
            if (tile_idx >= 0 && data->selected_tileset >= 0) {
                uint32_t tileset_idx = static_cast<uint32_t>(data->selected_tileset);
                
                if (tileset_idx < applyCode.tileAddress.size() &&
                    static_cast<uint32_t>(tile_idx) < applyCode.tileAddress[tileset_idx].size()) {
                    
                    uint32_t tile_address = applyCode.tileAddress[tileset_idx][tile_idx];
                    uint32_t tile_width = applyCode.tileWidth[tileset_idx][tile_idx];
                    uint32_t tile_height = applyCode.tileHeight[tileset_idx][tile_idx];
                    
                    if (tile_width > 0 && tile_height > 0) {
                        GdkPixbuf *tile_pixbuf = ConvertImageTo24bits(tile_address + 0x2000000, 
                                                                      tile_width, tile_height);
                        if (tile_pixbuf != NULL) {
                            GdkPixbuf *scaled = gdk_pixbuf_scale_simple(tile_pixbuf, 
                                                                        TILE_DISPLAY_SIZE, 
                                                                        TILE_DISPLAY_SIZE, 
                                                                        GDK_INTERP_NEAREST);
                            g_object_unref(tile_pixbuf);
                            
                            gdk_cairo_set_source_pixbuf(cr, scaled, px, py);
                            cairo_paint(cr);
                            g_object_unref(scaled);
                        }
                    }
                }
            }
            
            // Draw grid lines
            cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
            cairo_set_line_width(cr, 1.0);
            cairo_rectangle(cr, px, py, TILE_DISPLAY_SIZE, TILE_DISPLAY_SIZE);
            cairo_stroke(cr);
        }
    }
    
    return FALSE;
}

// Flood fill algorithm
static void flood_fill(LevelEditorData *data, int x, int y, int target_tile, int replacement_tile) {
    // Check bounds
    if (x < 0 || x >= data->map_width || y < 0 || y >= data->map_height) {
        return;
    }
    
    // Check if current tile is the target tile
    if (data->map_data[y][x] != target_tile) {
        return;
    }
    
    // Avoid infinite loop if target and replacement are the same
    if (target_tile == replacement_tile) {
        return;
    }
    
    // Replace the tile
    data->map_data[y][x] = replacement_tile;
    
    // Recursively fill adjacent tiles (4-way connectivity)
    flood_fill(data, x + 1, y, target_tile, replacement_tile);
    flood_fill(data, x - 1, y, target_tile, replacement_tile);
    flood_fill(data, x, y + 1, target_tile, replacement_tile);
    flood_fill(data, x, y - 1, target_tile, replacement_tile);
}

// Callback: Draw button clicked
static void on_draw_button_clicked(GtkButton *button, gpointer user_data) {
    LevelEditorData *data = static_cast<LevelEditorData*>(user_data);
    data->current_mode = DRAW_MODE;
    
    // Update button styles
    GtkStyleContext *draw_context = gtk_widget_get_style_context(data->draw_button);
    GtkStyleContext *fill_context = gtk_widget_get_style_context(data->fill_button);
    gtk_style_context_add_class(draw_context, "suggested-action");
    gtk_style_context_remove_class(fill_context, "suggested-action");
}

// Callback: Fill button clicked
static void on_fill_button_clicked(GtkButton *button, gpointer user_data) {
    LevelEditorData *data = static_cast<LevelEditorData*>(user_data);
    data->current_mode = FILL_MODE;
    
    // Update button styles
    GtkStyleContext *draw_context = gtk_widget_get_style_context(data->draw_button);
    GtkStyleContext *fill_context = gtk_widget_get_style_context(data->fill_button);
    gtk_style_context_remove_class(draw_context, "suggested-action");
    gtk_style_context_add_class(fill_context, "suggested-action");
}

// Callback: Generate Data button clicked
static void on_generate_data_clicked(GtkButton *button, gpointer user_data) {
    LevelEditorData *data = static_cast<LevelEditorData*>(user_data);
    
    // Generate data in assembly format: "db t,t,t,t,..."
    std::string output;
    
    for (int y = 0; y < data->map_height; y++) {
        output += "db ";
        for (int x = 0; x < data->map_width; x++) {
            output += std::to_string(data->map_data[y][x]);
            if (x < data->map_width - 1) {
                output += ",";
            }
        }
        output += "\n";
    }
    
    // Copy to clipboard
    GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
    gtk_clipboard_set_text(clipboard, output.c_str(), -1);
    
    // Show confirmation dialog
    GtkWidget *dialog = gtk_message_dialog_new(
        GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(button))),
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_OK,
        "Map data copied to clipboard!"
    );
    gtk_message_dialog_format_secondary_text(
        GTK_MESSAGE_DIALOG(dialog),
        "Paste the generated map data into your source code manually.\n\n"
        "Map size: %dx%d (%d lines)",
        data->map_width, data->map_height, data->map_height
    );
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

// Callback: click on map to draw tile
static gboolean on_map_clicked(GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
    LevelEditorData *data = static_cast<LevelEditorData*>(user_data);
    
    const int TILE_DISPLAY_SIZE = 32;
    
    int tile_x = static_cast<int>(event->x) / TILE_DISPLAY_SIZE;
    int tile_y = static_cast<int>(event->y) / TILE_DISPLAY_SIZE;
    
    if (tile_x >= 0 && tile_x < data->map_width && 
        tile_y >= 0 && tile_y < data->map_height) {
        
        // Right click: draw tile 0 (works in both modes)
        if (event->button == 3) {
            if (data->current_mode == FILL_MODE) {
                // Flood fill with tile 0
                int target_tile = data->map_data[tile_y][tile_x];
                flood_fill(data, tile_x, tile_y, target_tile, 0);
            } else {
                data->map_data[tile_y][tile_x] = 0;
            }
            gtk_widget_queue_draw(widget);
        }
        // Left click: draw selected tile or flood fill
        else if (event->button == 1 && data->selected_tile_index >= 0) {
            if (data->current_mode == FILL_MODE) {
                // Flood fill: replace tiles with value 0 by selected tile
                int target_tile = data->map_data[tile_y][tile_x];
                if (target_tile == 0) {
                    flood_fill(data, tile_x, tile_y, target_tile, data->selected_tile_index);
                }
            } else {
                data->map_data[tile_y][tile_x] = data->selected_tile_index;
            }
            gtk_widget_queue_draw(widget);
        }
    }
    
    return TRUE;
}

// Callback: mouse motion on map (for dragging)
static gboolean on_map_motion(GtkWidget *widget, GdkEventMotion *event, gpointer user_data) {
    LevelEditorData *data = static_cast<LevelEditorData*>(user_data);
    
    const int TILE_DISPLAY_SIZE = 32;
    
    int tile_x = static_cast<int>(event->x) / TILE_DISPLAY_SIZE;
    int tile_y = static_cast<int>(event->y) / TILE_DISPLAY_SIZE;
    
    if (tile_x >= 0 && tile_x < data->map_width && 
        tile_y >= 0 && tile_y < data->map_height) {
        
        // Right button drag: draw tile 0
        if (event->state & GDK_BUTTON3_MASK) {
            if (data->map_data[tile_y][tile_x] != 0) {
                data->map_data[tile_y][tile_x] = 0;
                gtk_widget_queue_draw(widget);
            }
            return TRUE;
        }
        // Left button drag: draw selected tile
        else if (event->state & GDK_BUTTON1_MASK && data->selected_tile_index >= 0) {
            if (data->map_data[tile_y][tile_x] != data->selected_tile_index) {
                data->map_data[tile_y][tile_x] = data->selected_tile_index;
                gtk_widget_queue_draw(widget);
            }
            return TRUE;
        }
    }
    
    return FALSE;
}

void open_level_editor(GtkWindow *parent_window, GtkWidget *message_label) {
    if (applyCode.tilesetCount == 0) {
        GtkWidget *dialog = gtk_message_dialog_new(parent_window,
                                                   GTK_DIALOG_MODAL,
                                                   GTK_MESSAGE_WARNING,
                                                   GTK_BUTTONS_OK,
                                                   "No Tilesets");
        gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog),
                                                 "Please import at least one tileset first.");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }
    
    // Create dialog window
    GtkWidget *dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(dialog), "Level Editor");
    gtk_window_set_default_size(GTK_WINDOW(dialog), 900, 600);
    gtk_window_set_transient_for(GTK_WINDOW(dialog), parent_window);
    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    
    // Create data structure
    LevelEditorData *data = g_new0(LevelEditorData, 1);
    data->selected_tileset = -1;
    data->selected_tile_index = -1;
    data->map_width = 10;
    data->map_height = 10;
    data->current_mode = DRAW_MODE; // Default mode is draw
    init_map_data(data);
    
    // Main horizontal box
    GtkWidget *main_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(main_hbox), 10);
    
    // LEFT PANEL
    GtkWidget *left_panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_size_request(left_panel, 280, -1);
    
    // Tileset selector
    GtkWidget *tileset_label = gtk_label_new("Select Tileset:");
    gtk_widget_set_halign(tileset_label, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(left_panel), tileset_label, FALSE, FALSE, 0);
    
    data->tileset_combo = gtk_combo_box_text_new();
    for (uint32_t i = 0; i < applyCode.tilesetCount; i++) {
        std::string label = std::to_string(i);
        if (i < applyCode.tilesetPath.size() && !applyCode.tilesetPath[i].empty()) {
            std::string path = applyCode.tilesetPath[i];
            size_t last_slash = path.find_last_of("/\\");
            std::string filename = (last_slash != std::string::npos) ? path.substr(last_slash + 1) : path;
            label += " - " + filename;
        }
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(data->tileset_combo), label.c_str());
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(data->tileset_combo), 0);
    data->selected_tileset = 0;
    gtk_box_pack_start(GTK_BOX(left_panel), data->tileset_combo, FALSE, FALSE, 0);
    
    // Tileset label and image (256x256)
    GtkWidget *tileset_label2 = gtk_label_new("Tileset (click to select):");
    gtk_widget_set_halign(tileset_label2, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(left_panel), tileset_label2, FALSE, FALSE, 0);
    
    GtkWidget *tileset_eventbox = gtk_event_box_new();
    data->tileset_image = gtk_image_new();
    gtk_widget_set_size_request(data->tileset_image, 256, 256);
    gtk_widget_add_events(data->tileset_image, GDK_BUTTON_PRESS_MASK);
    gtk_container_add(GTK_CONTAINER(tileset_eventbox), data->tileset_image);
    gtk_box_pack_start(GTK_BOX(left_panel), tileset_eventbox, FALSE, FALSE, 0);
    
    // Selected tile preview
    GtkWidget *selected_frame = gtk_frame_new("Selected Tile");
    data->selected_tile_image = gtk_image_new();
    gtk_widget_set_size_request(data->selected_tile_image, 64, 64);
    gtk_container_add(GTK_CONTAINER(selected_frame), data->selected_tile_image);
    gtk_box_pack_start(GTK_BOX(left_panel), selected_frame, FALSE, FALSE, 0);
    
    // RIGHT PANEL
    GtkWidget *right_panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    
    // Map size controls
    GtkWidget *size_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    
    GtkWidget *width_label = gtk_label_new("Width (tiles):");
    data->width_spinbutton = gtk_spin_button_new_with_range(1, 100, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(data->width_spinbutton), data->map_width);
    
    GtkWidget *height_label = gtk_label_new("Height (tiles):");
    data->height_spinbutton = gtk_spin_button_new_with_range(1, 100, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(data->height_spinbutton), data->map_height);
    
    gtk_box_pack_start(GTK_BOX(size_box), width_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(size_box), data->width_spinbutton, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(size_box), height_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(size_box), data->height_spinbutton, FALSE, FALSE, 0);
    
    gtk_box_pack_start(GTK_BOX(right_panel), size_box, FALSE, FALSE, 0);
    
    // Drawing mode buttons
    GtkWidget *mode_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *mode_label = gtk_label_new("Drawing Mode:");
    gtk_box_pack_start(GTK_BOX(mode_box), mode_label, FALSE, FALSE, 0);
    
    data->draw_button = gtk_button_new_with_label("Draw");
    data->fill_button = gtk_button_new_with_label("Fill");
    
    // Set initial button style (Draw is active by default)
    GtkStyleContext *draw_context = gtk_widget_get_style_context(data->draw_button);
    gtk_style_context_add_class(draw_context, "suggested-action");
    
    gtk_box_pack_start(GTK_BOX(mode_box), data->draw_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(mode_box), data->fill_button, FALSE, FALSE, 0);
    
    gtk_box_pack_start(GTK_BOX(right_panel), mode_box, FALSE, FALSE, 0);
    
    g_signal_connect(data->draw_button, "clicked", G_CALLBACK(on_draw_button_clicked), data);
    g_signal_connect(data->fill_button, "clicked", G_CALLBACK(on_fill_button_clicked), data);
    
    // Generate Data button
    GtkWidget *generate_button = gtk_button_new_with_label("Generate Data");
    gtk_box_pack_start(GTK_BOX(right_panel), generate_button, FALSE, FALSE, 0);
    g_signal_connect(generate_button, "clicked", G_CALLBACK(on_generate_data_clicked), data);
    
    // Map drawing area with scrolled window
    GtkWidget *map_frame = gtk_frame_new("Map");
    GtkWidget *map_scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(map_scrolled), 
                                   GTK_POLICY_AUTOMATIC, 
                                   GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_min_content_width(GTK_SCROLLED_WINDOW(map_scrolled), 400);
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(map_scrolled), 400);
    gtk_widget_set_size_request(map_scrolled, 400, 400);
    
    data->map_drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(data->map_drawing_area, 
                                data->map_width * 32, 
                                data->map_height * 32);
    gtk_widget_add_events(data->map_drawing_area, 
                          GDK_BUTTON_PRESS_MASK | 
                          GDK_BUTTON1_MOTION_MASK |
                          GDK_BUTTON3_MOTION_MASK);
    
    // Use viewport for proper scrolling with drawing area
    GtkWidget *viewport = gtk_viewport_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(viewport), data->map_drawing_area);
    gtk_container_add(GTK_CONTAINER(map_scrolled), viewport);
    gtk_container_add(GTK_CONTAINER(map_frame), map_scrolled);
    gtk_box_pack_start(GTK_BOX(right_panel), map_frame, TRUE, TRUE, 0);
    
    // Pack panels
    gtk_box_pack_start(GTK_BOX(main_hbox), left_panel, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(main_hbox), right_panel, TRUE, TRUE, 0);
    
    gtk_container_add(GTK_CONTAINER(dialog), main_hbox);
    
    // Connect signals
    g_signal_connect(data->tileset_combo, "changed", G_CALLBACK(on_tileset_combo_changed), data);
    g_signal_connect(tileset_eventbox, "button-press-event", G_CALLBACK(on_tileset_image_clicked), data);
    g_signal_connect(data->width_spinbutton, "value-changed", G_CALLBACK(on_map_size_changed), data);
    g_signal_connect(data->height_spinbutton, "value-changed", G_CALLBACK(on_map_size_changed), data);
    g_signal_connect(data->map_drawing_area, "draw", G_CALLBACK(on_map_draw), data);
    g_signal_connect(data->map_drawing_area, "button-press-event", G_CALLBACK(on_map_clicked), data);
    g_signal_connect(data->map_drawing_area, "motion-notify-event", G_CALLBACK(on_map_motion), data);
    
    g_signal_connect_swapped(dialog, "destroy", G_CALLBACK(g_free), data);
    
    // Initialize display
    update_tileset_image(data);
    
    gtk_widget_show_all(dialog);
    
    if (message_label != NULL) {
        gtk_label_set_text(GTK_LABEL(message_label), "Level Editor opened");
    }
}