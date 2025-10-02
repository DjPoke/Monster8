#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <cstdint>
#include <fstream>
#include <functional>
#include <limits>
#include <sstream>
#include <string>
#include <vector>
#include <deque>
#include <cerrno>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <map>
#include <cmath>
#include <filesystem>

#include "code.hpp"
#include "gtk_p1.hpp"

using namespace std;

// instantiate
ApplyCode applyCode;
GtkWidget *main_text_view = NULL;
std::string g_last_search_text;
GtkTextSearchFlags g_last_search_flags = GTK_TEXT_SEARCH_TEXT_ONLY;

std::vector<std::string> g_undo_stack;
std::vector<std::string> g_redo_stack;
std::string g_current_text;
bool g_is_applying_history = false;
const std::size_t g_max_history_entries = 256;

void
update_cursor_position(GtkTextBuffer *buffer, GtkWidget *cursor_label)
{
    GtkTextIter cursor_iter;
    gtk_text_buffer_get_iter_at_mark(buffer, &cursor_iter, gtk_text_buffer_get_insert(buffer));

    int line_number = gtk_text_iter_get_line(&cursor_iter) + 1;
    int column_number = gtk_text_iter_get_line_offset(&cursor_iter) + 1;

    char status_text[64];
    g_snprintf(status_text, sizeof(status_text), "Line %d, Column %d", line_number, column_number);
    gtk_label_set_text(GTK_LABEL(cursor_label), status_text);
}

void
on_cursor_mark_set(GtkTextBuffer *buffer, GtkTextIter *location, GtkTextMark *mark, gpointer user_data)
{
    GtkTextMark *insert_mark = gtk_text_buffer_get_insert(buffer);

    if (mark == insert_mark) {
        GtkWidget *cursor_label = GTK_WIDGET(user_data);
        update_cursor_position(buffer, cursor_label);
    }
}

void
on_cursor_position_notify(GObject *object, GParamSpec *pspec, gpointer user_data)
{
    GtkTextBuffer *buffer = GTK_TEXT_BUFFER(object);
    GtkWidget *cursor_label = GTK_WIDGET(user_data);
    update_cursor_position(buffer, cursor_label);
}

// structure Accelerator (defined in gtk.hpp)

// Fonction qui associe un ensemble d’accélérateurs à tes menu items
void bind_menu_accelerators(GtkWidget* window,
                            const std::vector<GtkWidget*>& items,
                            const std::vector<Accelerator>& accels)
{
    GtkAccelGroup* accel_group = gtk_accel_group_new();
    gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);

    // Vérification que les vecteurs correspondent
    size_t count = std::min(items.size(), accels.size());

    for (size_t i = 0; i < count; ++i) {
        gtk_widget_add_accelerator(items[i], "activate", accel_group,
                                   accels[i].key, accels[i].mod, GTK_ACCEL_VISIBLE);
    }
}

static void
on_activate(GtkApplication* app)
{
    // Création de la fenêtre principale
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Monster8 Editor");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER_ALWAYS);
    
    // Création d'un conteneur vertical
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), box);

    // Barre de menu
    GtkWidget *menu_bar = gtk_menu_bar_new();

    GtkWidget *project_menu_item = gtk_menu_item_new_with_label("Project");
    GtkWidget *project_menu = gtk_menu_new();
    GtkWidget *project_new_item = gtk_menu_item_new_with_label("New");
    GtkWidget *project_open_item = gtk_menu_item_new_with_label("Open");
    GtkWidget *project_save_item = gtk_menu_item_new_with_label("Save");
    GtkWidget *project_save_as_item = gtk_menu_item_new_with_label("Save As...");
    GtkWidget *project_export_rom_item = gtk_menu_item_new_with_label("Export ROM");
    GtkWidget *project_quit_item = gtk_menu_item_new_with_label("Quit");

    GtkWidget *edit_menu_item = gtk_menu_item_new_with_label("Edit");
    GtkWidget *edit_menu = gtk_menu_new();
    GtkWidget *edit_undo_item = gtk_menu_item_new_with_label("Undo");
    GtkWidget *edit_redo_item = gtk_menu_item_new_with_label("Redo");
    GtkWidget *edit_select_all_item = gtk_menu_item_new_with_label("Select All");
    GtkWidget *edit_search_item = gtk_menu_item_new_with_label("Search");
    GtkWidget *edit_replace_item = gtk_menu_item_new_with_label("Replace");
    GtkWidget *edit_goto_line_item = gtk_menu_item_new_with_label("Goto Line");

    GtkWidget *compiler_menu_item = gtk_menu_item_new_with_label("Compiler");
    GtkWidget *compiler_menu = gtk_menu_new();
    GtkWidget *compiler_build_item = gtk_menu_item_new_with_label("Build");
    GtkWidget *compiler_build_and_run_item = gtk_menu_item_new_with_label("Build and run");

    GtkWidget *medias_menu_item = gtk_menu_item_new_with_label("Medias");
    GtkWidget *medias_menu = gtk_menu_new();
    GtkWidget *medias_clear_sprites_item = gtk_menu_item_new_with_label("Clear sprites");
    GtkWidget *medias_clear_tilesets_item = gtk_menu_item_new_with_label("Clear tilesets");
    GtkWidget *medias_clear_sounds_item = gtk_menu_item_new_with_label("Clear sounds");
    GtkWidget *medias_clear_musics_item = gtk_menu_item_new_with_label("Clear musics");
    GtkWidget *medias_import_spritesheet_item = gtk_menu_item_new_with_label("Import sprite sheet");
    GtkWidget *medias_import_tileset_item = gtk_menu_item_new_with_label("Import tileset");
    GtkWidget *medias_import_sound_item = gtk_menu_item_new_with_label("Import sound");
    GtkWidget *medias_import_music_item = gtk_menu_item_new_with_label("Import music");
    GtkWidget *medias_informations_item = gtk_menu_item_new_with_label("Medias Informations");

    GtkWidget *tools_menu_item = gtk_menu_item_new_with_label("Tools");
    GtkWidget *tools_menu = gtk_menu_new();
    GtkWidget *tools_level_editor_item = gtk_menu_item_new_with_label("Level editor");
    GtkWidget *tools_sentence_editor_item = gtk_menu_item_new_with_label("Sentence editor");

    GtkWidget *settings_menu_item = gtk_menu_item_new_with_label("Settings");
    GtkWidget *settings_menu = gtk_menu_new();
    GtkWidget *settings_setup_emulator_item = gtk_menu_item_new_with_label("Set Monster8 emulator path");

    GtkWidget *informations_menu_item = gtk_menu_item_new_with_label("?");
    GtkWidget *informations_menu = gtk_menu_new();
    GtkWidget *informations_about_item = gtk_menu_item_new_with_label("About Monster8 Editor");

    GtkWidget *project_separator1 = gtk_separator_menu_item_new();
    GtkWidget *project_separator2 = gtk_separator_menu_item_new();
    GtkWidget *project_separator3 = gtk_separator_menu_item_new();

    GtkWidget *edit_separator1 = gtk_separator_menu_item_new();
    GtkWidget *edit_separator2 = gtk_separator_menu_item_new();
    
    GtkWidget *medias_separator1 = gtk_separator_menu_item_new();
    GtkWidget *medias_separator2 = gtk_separator_menu_item_new();

    gtk_menu_shell_append(GTK_MENU_SHELL(project_menu), project_new_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(project_menu), project_separator1);
    gtk_menu_shell_append(GTK_MENU_SHELL(project_menu), project_open_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(project_menu), project_save_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(project_menu), project_save_as_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(project_menu), project_separator2);
    gtk_menu_shell_append(GTK_MENU_SHELL(project_menu), project_export_rom_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(project_menu), project_separator3);
    gtk_menu_shell_append(GTK_MENU_SHELL(project_menu), project_quit_item);

    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), edit_undo_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), edit_redo_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), edit_separator1);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), edit_select_all_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), edit_separator2);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), edit_search_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), edit_replace_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), edit_goto_line_item);

    gtk_menu_shell_append(GTK_MENU_SHELL(compiler_menu), compiler_build_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(compiler_menu), compiler_build_and_run_item);


    gtk_menu_shell_append(GTK_MENU_SHELL(medias_menu), medias_clear_sprites_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(medias_menu), medias_clear_tilesets_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(medias_menu), medias_clear_sounds_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(medias_menu), medias_clear_musics_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(medias_menu), medias_separator1);
    gtk_menu_shell_append(GTK_MENU_SHELL(medias_menu), medias_import_spritesheet_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(medias_menu), medias_import_tileset_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(medias_menu), medias_import_sound_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(medias_menu), medias_import_music_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(medias_menu), medias_separator2);
    gtk_menu_shell_append(GTK_MENU_SHELL(medias_menu), medias_informations_item);

    gtk_menu_shell_append(GTK_MENU_SHELL(tools_menu), tools_level_editor_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(tools_menu), tools_sentence_editor_item);

    gtk_menu_shell_append(GTK_MENU_SHELL(settings_menu), settings_setup_emulator_item);

    gtk_menu_shell_append(GTK_MENU_SHELL(informations_menu), informations_about_item);

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(project_menu_item), project_menu);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(edit_menu_item), edit_menu);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(compiler_menu_item), compiler_menu);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(medias_menu_item), medias_menu);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(tools_menu_item), tools_menu);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(settings_menu_item), settings_menu);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(informations_menu_item), informations_menu);

    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), project_menu_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), edit_menu_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), compiler_menu_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), medias_menu_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), tools_menu_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), settings_menu_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), informations_menu_item);

    gtk_box_pack_start(GTK_BOX(box), menu_bar, FALSE, FALSE, 0);

    // Création d'un widget de défilement pour l'éditeur de texte
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_vexpand(scrolled_window, TRUE);
    gtk_box_pack_start(GTK_BOX(box), scrolled_window, TRUE, TRUE, 0);

    // Création de l'éditeur de texte enrichi (GtkTextView)
    GtkWidget *text_view = gtk_text_view_new();
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD_CHAR);
    gtk_container_add(GTK_CONTAINER(scrolled_window), text_view);
    main_text_view = text_view;
    load_editor_css(text_view);

    // Barre de statut divisée en deux zones
    GtkWidget *status_bar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_margin_top(status_bar, 5);
    gtk_widget_set_margin_bottom(status_bar, 5);
    gtk_box_pack_start(GTK_BOX(box), status_bar, FALSE, FALSE, 0);

    GtkWidget *cursor_status_label = gtk_label_new("Line 1, Column 1");
    gtk_box_pack_start(GTK_BOX(status_bar), cursor_status_label, FALSE, FALSE, 10);

    GtkWidget *message_status_label = gtk_label_new("Ready");
    gtk_widget_set_hexpand(message_status_label, TRUE);
    gtk_label_set_xalign(GTK_LABEL(message_status_label), 1.0f);
    gtk_box_pack_start(GTK_BOX(status_bar), message_status_label, TRUE, TRUE, 10);
    g_object_set_data(G_OBJECT(text_view), "message-status-label", message_status_label);

    // Récupération du buffer de texte
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    ensure_highlight_tags(buffer);
    g_signal_connect(buffer, "mark-set", G_CALLBACK(on_cursor_mark_set), cursor_status_label);
    g_signal_connect(buffer, "notify::cursor-position", G_CALLBACK(on_cursor_position_notify), cursor_status_label);
    g_signal_connect(buffer, "changed", G_CALLBACK(on_buffer_changed), message_status_label);
    apply_editor_highlighting(buffer);
    update_cursor_position(buffer, cursor_status_label);

    // Initialise l'état de l'historique avec le contenu initial du buffer
    GtkTextIter init_start;
    GtkTextIter init_end;
    gtk_text_buffer_get_start_iter(buffer, &init_start);
    gtk_text_buffer_get_end_iter(buffer, &init_end);
    gchar *initial_text_raw = gtk_text_buffer_get_text(buffer, &init_start, &init_end, FALSE);
    g_current_text = initial_text_raw != NULL ? initial_text_raw : "";
    g_free(initial_text_raw);

    g_signal_connect(project_new_item, "activate", G_CALLBACK(on_project_new_activate), text_view);
    g_signal_connect(project_open_item, "activate", G_CALLBACK(on_project_open_activate), text_view);
    g_signal_connect(project_save_item, "activate", G_CALLBACK(on_project_save_activate), NULL);
    g_signal_connect(project_save_as_item, "activate", G_CALLBACK(on_project_save_as_activate), NULL);
    g_signal_connect(project_export_rom_item, "activate", G_CALLBACK(on_project_export_rom_activate), NULL);
    g_signal_connect(project_quit_item, "activate", G_CALLBACK(on_project_quit_activate), window);

    g_signal_connect(edit_undo_item, "activate", G_CALLBACK(on_edit_undo_activate), message_status_label);
    g_signal_connect(edit_redo_item, "activate", G_CALLBACK(on_edit_redo_activate), message_status_label);
    g_signal_connect(edit_select_all_item, "activate", G_CALLBACK(on_edit_select_all_activate), message_status_label);
    g_signal_connect(edit_search_item, "activate", G_CALLBACK(on_edit_search_activate), message_status_label);
    g_signal_connect(edit_replace_item, "activate", G_CALLBACK(on_edit_replace_activate), message_status_label);
    g_signal_connect(edit_goto_line_item, "activate", G_CALLBACK(on_edit_goto_line_activate), message_status_label);

    g_signal_connect(compiler_build_item, "activate", G_CALLBACK(on_compiler_build_activate), message_status_label);
    g_signal_connect(compiler_build_and_run_item, "activate", G_CALLBACK(on_compiler_build_and_run_activate), message_status_label);

    g_signal_connect(medias_clear_sprites_item, "activate", G_CALLBACK(on_medias_clear_sprites_activate), message_status_label);
    g_signal_connect(medias_clear_tilesets_item, "activate", G_CALLBACK(on_medias_clear_tilesets_activate), message_status_label);
    g_signal_connect(medias_clear_sounds_item, "activate", G_CALLBACK(on_medias_clear_sounds_activate), message_status_label);
    g_signal_connect(medias_clear_musics_item, "activate", G_CALLBACK(on_medias_clear_musics_activate), message_status_label);
    
    g_signal_connect(medias_import_spritesheet_item, "activate", G_CALLBACK(on_medias_import_spritesheet_activate), message_status_label);
    g_signal_connect(medias_import_tileset_item, "activate", G_CALLBACK(on_medias_import_tileset_activate), message_status_label);
    g_signal_connect(medias_import_sound_item, "activate", G_CALLBACK(on_medias_import_sound_activate), message_status_label);
    g_signal_connect(medias_import_music_item, "activate", G_CALLBACK(on_medias_import_music_activate), message_status_label);
    g_signal_connect(medias_informations_item, "activate", G_CALLBACK(on_medias_informations_activate), text_view);

    g_signal_connect(settings_setup_emulator_item, "activate", G_CALLBACK(on_settings_setup_emulator_activate), message_status_label);

    g_signal_connect(tools_level_editor_item, "activate", G_CALLBACK(on_tools_level_editor_activate), message_status_label);
    g_signal_connect(tools_sentence_editor_item, "activate", G_CALLBACK(on_tools_sentence_editor_activate), message_status_label);

    g_signal_connect(informations_about_item, "activate", G_CALLBACK(on_informations_about_activate), text_view);

    std::vector<GtkWidget*> menu_items = {
        project_new_item,
        project_open_item,
        project_save_item,
        project_save_as_item,
        project_export_rom_item,
        edit_undo_item,
        edit_redo_item,
        edit_select_all_item,
        edit_search_item,
        edit_replace_item,
        edit_goto_line_item,
        compiler_build_item,
        compiler_build_and_run_item,
        medias_import_spritesheet_item,
        medias_import_tileset_item,
        medias_import_sound_item,
        medias_import_music_item,
        medias_informations_item,
        project_quit_item,
        edit_search_item  // F3 Search Next binding
    };

    // définition des raccourcis correspondants
    std::vector<Accelerator> accels = {
        {GDK_KEY_n, GDK_CONTROL_MASK},   // Project New
        {GDK_KEY_o, GDK_CONTROL_MASK},   // Project Open
        {GDK_KEY_s, GDK_CONTROL_MASK},   // Project Save
        {GDK_KEY_s, static_cast<GdkModifierType>(GDK_CONTROL_MASK | GDK_SHIFT_MASK)},   // Project Save As
        {GDK_KEY_e, GDK_CONTROL_MASK},   // Project Export ROM
        {GDK_KEY_z, GDK_CONTROL_MASK},   // Edit Undo
        {GDK_KEY_y, GDK_CONTROL_MASK},   // Edit Redo
        {GDK_KEY_a, GDK_CONTROL_MASK},   // Edit Select All
        {GDK_KEY_f, GDK_CONTROL_MASK},   // Edit Search
        {GDK_KEY_h, GDK_CONTROL_MASK},   // Edit Replace
        {GDK_KEY_g, GDK_CONTROL_MASK},   // Edit Goto Line
        {GDK_KEY_F5, (GdkModifierType)0},  // Compiler Build
        {GDK_KEY_F6, (GdkModifierType)0},  // Compiler Build & Run
        {GDK_KEY_F7, (GdkModifierType)0},  // Medias Import Spritesheet
        {GDK_KEY_F8, (GdkModifierType)0},  // Medias Import Tileset
        {GDK_KEY_F9, (GdkModifierType)0},  // Medias Import Sound
        {GDK_KEY_F10, (GdkModifierType)0}, // Medias Import Music
        {GDK_KEY_i, GDK_CONTROL_MASK},   // Medias Informations
        {GDK_KEY_q, GDK_CONTROL_MASK},   // Project Quit
        {GDK_KEY_F3, (GdkModifierType)0}   // Edit Search Next
    };

    // Application
    bind_menu_accelerators(window, menu_items, accels);

    // affichage de la fenêtre
    gtk_widget_show_all(window);
}

int main(int argc, char **argv)
{
    const string settings_path = "Settings.cfg";
    bool should_initialize_settings = true;

    ifstream settings_in(settings_path);
    if (settings_in) {
        if (settings_in.peek() != ifstream::traits_type::eof()) {
            should_initialize_settings = false;
            string line;
            while (getline(settings_in, line)) {
                const string prefix = "EmulatorPath=";
                if (line.rfind(prefix, 0) == 0) {
                    applyCode.emuFile = line.substr(prefix.size());
                    break;
                }
            }
        }
    }

    if (settings_in.is_open()) {
        settings_in.close();
    }

    if (should_initialize_settings) {
        ofstream settings_out(settings_path, ios::out | ios::trunc);
        if (settings_out) {
            settings_out << "EmulatorPath=";
        }
    }

    // initialisation de l'application GTK
    GtkApplication *app = gtk_application_new("com.example.monster8editor", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);
    
    // exécution de l'application
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    
    return status;
}
