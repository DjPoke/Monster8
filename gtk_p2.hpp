#ifndef GTK_P2_FUNCTIONS_HPP
#define GTK_P2_FUNCTIONS_HPP

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <cstdint>
#include <string>
#include <vector>

#include "code.hpp"
#include "gtk_p1.hpp"

// External variable declarations
extern ApplyCode applyCode;
extern GtkWidget *main_text_view;
extern const std::size_t g_max_history_entries;
extern std::vector<std::string> g_undo_stack;
extern std::vector<std::string> g_redo_stack;
extern std::string g_current_text;
extern bool g_is_applying_history;
extern std::string g_last_search_text;
extern GtkTextSearchFlags g_last_search_flags;

// Structures pour les données des callbacks
struct SpriteViewerData {
    GtkWidget *combo;
    GtkWidget *image_widget;
};

struct TileViewerData {
    GtkWidget *combo;
    GtkWidget *image_widget;
};

struct Accelerator {
    guint key;
    GdkModifierType mod;
};

// Dialog functions
bool prompt_for_frame_dimensions(GtkWindow *parent_window, gint image_width, gint image_height, gint &frame_width, gint &frame_height);
void ShowMediaStatus(GtkWindow *parent_window);

// Text editor functions
void ensure_highlight_tags(GtkTextBuffer *buffer);
void apply_editor_highlighting(GtkTextBuffer *buffer);
void load_editor_css(GtkWidget *text_view);
void on_buffer_changed(GtkTextBuffer *buffer, gpointer user_data);
void on_search_dialog_destroy(GtkWidget *widget, gpointer user_data);

// Sprite viewer functions
void open_sprite_viewer(GtkWindow *parent_window);
void on_sprites_more_clicked(GtkButton *button, gpointer user_data);
void update_sprite_image(GtkWidget *combo, GtkWidget *image_widget);
void on_sprite_combo_changed(GtkComboBox *combo, gpointer user_data);

// Tile viewer functions  
void open_tile_viewer(GtkWindow *parent_window);
void on_tilesets_more_clicked(GtkButton *button, gpointer user_data);
void update_tile_image(GtkWidget *combo, GtkWidget *image_widget);
void on_tile_combo_changed(GtkComboBox *combo, gpointer user_data);

// Cursor and text functions
void update_cursor_position(GtkTextBuffer *buffer, GtkWidget *cursor_label);
void on_cursor_mark_set(GtkTextBuffer *buffer, GtkTextIter *location, GtkTextMark *mark, gpointer user_data);
void on_cursor_position_notify(GObject *object, GParamSpec *pspec, gpointer user_data);

// Menu and accelerator functions
void bind_menu_accelerators(GtkWidget* window,
                            const std::vector<GtkWidget*>& items,
                            const std::vector<Accelerator>& accels);

// Menu callback functions
void on_project_new_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_project_open_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_project_save_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_project_save_as_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_project_export_rom_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_project_quit_activate(GtkMenuItem *menuitem, gpointer user_data);

void on_edit_undo_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_edit_redo_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_edit_select_all_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_edit_search_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_edit_replace_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_edit_goto_line_activate(GtkMenuItem *menuitem, gpointer user_data);

void on_compiler_build_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_compiler_build_and_run_activate(GtkMenuItem *menuitem, gpointer user_data);

void on_medias_clear_sprites_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_medias_clear_tilesets_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_medias_clear_sounds_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_medias_clear_musics_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_medias_import_spritesheet_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_medias_import_tileset_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_medias_import_sound_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_medias_import_music_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_medias_informations_activate(GtkMenuItem *menuitem, gpointer user_data);

void on_settings_setup_emulator_activate(GtkMenuItem *menuitem, gpointer user_data);

void on_tools_level_editor_activate(GtkMenuItem *menuitem, gpointer user_data);

void on_informations_about_activate(GtkMenuItem *menuitem, gpointer user_data);

#endif // GTK_FUNCTIONS_HPP