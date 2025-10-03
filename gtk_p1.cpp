#include <fstream>
#include <sstream>
#include <filesystem>
#include <deque>
#include <cerrno>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <map>
#include <cmath>

#include "gtk_p1.hpp"

using namespace std;

// load functions
void OpenProject(GtkWindow *parent_window, GtkWidget *text_view) {
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Open Asm8 Project",
                                                    parent_window,
                                                    GTK_FILE_CHOOSER_ACTION_OPEN,
                                                    "_Cancel", GTK_RESPONSE_CANCEL,
                                                    "_Open", GTK_RESPONSE_ACCEPT,
                                                    NULL);

    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "Asm8 files (*.asm8)");
    gtk_file_filter_add_pattern(filter, "*.asm8");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

        if (filename != NULL) {
            ifstream asm_file(filename);

            if (asm_file) {
                std::string bin_path_line;
                std::getline(asm_file, bin_path_line);
                
                // Supprimer espaceopen_s et retour chariot en fin de ligne
                while (!bin_path_line.empty() && (isspace(bin_path_line.back()) || bin_path_line.back() == '\r' || bin_path_line.back() == '\n')) {
                    bin_path_line.pop_back();
                }

                if (!bin_path_line.empty()) {
                    applyCode.binFile = bin_path_line;

                    LoadROM(applyCode.binFile);
                }

                auto parse_next_uint32 = [&](uint32_t &output) -> bool {
                    string line;
                    if (!getline(asm_file, line)) {
                        return false;
                    }

                    errno = 0;
                    char *end_ptr = nullptr;
                    unsigned long value = strtoul(line.c_str(), &end_ptr, 10);

                    if (errno != 0 || end_ptr == line.c_str()) {
                        return false;
                    }

                    while (*end_ptr != '\0' && isspace(static_cast<unsigned char>(*end_ptr))) {
                        ++end_ptr;
                    }

                    if (*end_ptr != '\0') {
                        return false;
                    }

                    if (value > numeric_limits<uint32_t>::max()) {
                        return false;
                    }

                    output = static_cast<uint32_t>(value);
                    return true;
                };

                // Clear existing vectors
                applyCode.spritesheetPath.clear();
                applyCode.spritesheetAddress.clear();
                applyCode.spritesheetLength.clear();
                applyCode.spritesheetWidth.clear();
                applyCode.spritesheetHeight.clear();
                applyCode.spriteCount.clear();
                applyCode.spriteAddress.clear();
                applyCode.spriteLength.clear();
                applyCode.spriteWidth.clear();
                applyCode.spriteHeight.clear();
                applyCode.tilesetPath.clear();
                applyCode.tilesetAddress.clear();
                applyCode.tilesetLength.clear();
                applyCode.tilesetWidth.clear();
                applyCode.tilesetHeight.clear();
                applyCode.tileCount.clear();
                applyCode.tileAddress.clear();
                applyCode.tileLength.clear();
                applyCode.tileWidth.clear();
                applyCode.tileHeight.clear();
                applyCode.soundPath.clear();
                applyCode.soundAddress.clear();
                applyCode.soundLength.clear();
                applyCode.musicPath.clear();
                applyCode.musicAddress.clear();
                applyCode.musicLength.clear();

                if (!parse_next_uint32(applyCode.spritesheetCount) ||
                    !parse_next_uint32(applyCode.tilesetCount) ||
                    !parse_next_uint32(applyCode.soundCount) ||
                    !parse_next_uint32(applyCode.musicCount)) {
                    GtkWidget *message_label = GTK_WIDGET(g_object_get_data(G_OBJECT(text_view), "message-status-label"));
                    if (message_label != NULL) {
                        gtk_label_set_text(GTK_LABEL(message_label), "Invalid Asm8 file: bad header values");
                    }
                    asm_file.close();
                    g_free(filename);
                    gtk_widget_destroy(dialog);
                    return;
                }

                // Read spritesheet data
                for (uint32_t i = 0; i < applyCode.spritesheetCount; i++) {
                    string spritesheet_path, spritesheet_addr_str, spritesheet_len_str, spritesheet_width_str, spritesheet_height_str;
                    if (!getline(asm_file, spritesheet_path) ||
                        !getline(asm_file, spritesheet_addr_str) ||
                        !getline(asm_file, spritesheet_len_str) ||
                        !getline(asm_file, spritesheet_width_str) ||
                        !getline(asm_file, spritesheet_height_str)) {
                        GtkWidget *message_label = GTK_WIDGET(g_object_get_data(G_OBJECT(text_view), "message-status-label"));
                        if (message_label != NULL) {
                            gtk_label_set_text(GTK_LABEL(message_label), "Invalid Asm8 file: incomplete spritesheet data");
                        }
                        asm_file.close();
                        g_free(filename);
                        gtk_widget_destroy(dialog);
                        return;
                    }
                    
                    uint32_t spritesheet_addr = std::stoul(spritesheet_addr_str);
                    uint32_t spritesheet_len = std::stoul(spritesheet_len_str);
                    uint32_t spritesheet_width = std::stoul(spritesheet_width_str);
                    uint32_t spritesheet_height = std::stoul(spritesheet_height_str);
                    
                    applyCode.spritesheetPath.push_back(spritesheet_path);
                    applyCode.spritesheetAddress.push_back(spritesheet_addr);
                    applyCode.spritesheetLength.push_back(spritesheet_len);
                    applyCode.spritesheetWidth.push_back(spritesheet_width);
                    applyCode.spritesheetHeight.push_back(spritesheet_height);
                    
                    // Read sprite count for this spritesheet
                    uint32_t sprite_count_in_sheet = 0;
                    if (!parse_next_uint32(sprite_count_in_sheet)) {
                        GtkWidget *message_label = GTK_WIDGET(g_object_get_data(G_OBJECT(text_view), "message-status-label"));
                        if (message_label != NULL) {
                            gtk_label_set_text(GTK_LABEL(message_label), "Invalid Asm8 file: bad sprite count in spritesheet");
                        }
                        asm_file.close();
                        g_free(filename);
                        gtk_widget_destroy(dialog);
                        return;
                    }
                    applyCode.spriteCount.push_back(sprite_count_in_sheet);
                    
                    // Read individual sprites for this spritesheet
                    std::vector<uint32_t> sprite_addresses;
                    std::vector<uint32_t> sprite_lengths;
                    std::vector<uint32_t> sprite_widths;
                    std::vector<uint32_t> sprite_heights;
                    
                    for (uint32_t j = 0; j < sprite_count_in_sheet; j++) {
                        string sprite_addr_str, sprite_len_str, sprite_width_str, sprite_height_str;
                        if (!getline(asm_file, sprite_addr_str) ||
                            !getline(asm_file, sprite_len_str) ||
                            !getline(asm_file, sprite_width_str) ||
                            !getline(asm_file, sprite_height_str)) {
                            GtkWidget *message_label = GTK_WIDGET(g_object_get_data(G_OBJECT(text_view), "message-status-label"));
                            if (message_label != NULL) {
                                gtk_label_set_text(GTK_LABEL(message_label), "Invalid Asm8 file: incomplete sprite data");
                            }
                            asm_file.close();
                            g_free(filename);
                            gtk_widget_destroy(dialog);
                            return;
                        }
                        
                        sprite_addresses.push_back(std::stoul(sprite_addr_str));
                        sprite_lengths.push_back(std::stoul(sprite_len_str));
                        sprite_widths.push_back(std::stoul(sprite_width_str));
                        sprite_heights.push_back(std::stoul(sprite_height_str));
                    }
                    
                    applyCode.spriteAddress.push_back(sprite_addresses);
                    applyCode.spriteLength.push_back(sprite_lengths);
                    applyCode.spriteWidth.push_back(sprite_widths);
                    applyCode.spriteHeight.push_back(sprite_heights);
                }

                // Read tileset data
                for (uint32_t i = 0; i < applyCode.tilesetCount; i++) {
                    string tileset_path, tileset_addr_str, tileset_len_str, tileset_width_str, tileset_height_str;
                    if (!getline(asm_file, tileset_path) ||
                        !getline(asm_file, tileset_addr_str) ||
                        !getline(asm_file, tileset_len_str) ||
                        !getline(asm_file, tileset_width_str) ||
                        !getline(asm_file, tileset_height_str)) {
                        GtkWidget *message_label = GTK_WIDGET(g_object_get_data(G_OBJECT(text_view), "message-status-label"));
                        if (message_label != NULL) {
                            gtk_label_set_text(GTK_LABEL(message_label), "Invalid Asm8 file: incomplete tileset data");
                        }
                        asm_file.close();
                        g_free(filename);
                        gtk_widget_destroy(dialog);
                        return;
                    }
                    
                    uint32_t tileset_addr = std::stoul(tileset_addr_str);
                    uint32_t tileset_len = std::stoul(tileset_len_str);
                    uint32_t tileset_width = std::stoul(tileset_width_str);
                    uint32_t tileset_height = std::stoul(tileset_height_str);
                    
                    applyCode.tilesetPath.push_back(tileset_path);
                    applyCode.tilesetAddress.push_back(tileset_addr);
                    applyCode.tilesetLength.push_back(tileset_len);
                    applyCode.tilesetWidth.push_back(tileset_width);
                    applyCode.tilesetHeight.push_back(tileset_height);
                    
                    // Read tile count for this tileset
                    uint32_t tile_count_in_set = 0;
                    if (!parse_next_uint32(tile_count_in_set)) {
                        GtkWidget *message_label = GTK_WIDGET(g_object_get_data(G_OBJECT(text_view), "message-status-label"));
                        if (message_label != NULL) {
                            gtk_label_set_text(GTK_LABEL(message_label), "Invalid Asm8 file: bad tile count in tileset");
                        }
                        asm_file.close();
                        g_free(filename);
                        gtk_widget_destroy(dialog);
                        return;
                    }
                    applyCode.tileCount.push_back(tile_count_in_set);
                    
                    // Read individual tiles for this tileset
                    std::vector<uint32_t> tile_addresses;
                    std::vector<uint32_t> tile_lengths;
                    std::vector<uint32_t> tile_widths;
                    std::vector<uint32_t> tile_heights;
                    
                    for (uint32_t j = 0; j < tile_count_in_set; j++) {
                        string tile_addr_str, tile_len_str, tile_width_str, tile_height_str;
                        if (!getline(asm_file, tile_addr_str) ||
                            !getline(asm_file, tile_len_str) ||
                            !getline(asm_file, tile_width_str) ||
                            !getline(asm_file, tile_height_str)) {
                            GtkWidget *message_label = GTK_WIDGET(g_object_get_data(G_OBJECT(text_view), "message-status-label"));
                            if (message_label != NULL) {
                                gtk_label_set_text(GTK_LABEL(message_label), "Invalid Asm8 file: incomplete tile data");
                            }
                            asm_file.close();
                            g_free(filename);
                            gtk_widget_destroy(dialog);
                            return;
                        }
                        
                        tile_addresses.push_back(std::stoul(tile_addr_str));
                        tile_lengths.push_back(std::stoul(tile_len_str));
                        tile_widths.push_back(std::stoul(tile_width_str));
                        tile_heights.push_back(std::stoul(tile_height_str));
                    }
                    
                    applyCode.tileAddress.push_back(tile_addresses);
                    applyCode.tileLength.push_back(tile_lengths);
                    applyCode.tileWidth.push_back(tile_widths);
                    applyCode.tileHeight.push_back(tile_heights);
                }

                // Read sound count and data
                uint32_t sound_count = 0;
                if (!parse_next_uint32(sound_count)) {
                    GtkWidget *message_label = GTK_WIDGET(g_object_get_data(G_OBJECT(text_view), "message-status-label"));
                    if (message_label != NULL) {
                        gtk_label_set_text(GTK_LABEL(message_label), "Invalid Asm8 file: bad sound count");
                    }
                    asm_file.close();
                    g_free(filename);
                    gtk_widget_destroy(dialog);
                    return;
                }

                for (uint32_t i = 0; i < sound_count; i++) {
                    string sound_path, sound_addr_str, sound_len_str;
                    if (!getline(asm_file, sound_path) ||
                        !getline(asm_file, sound_addr_str) ||
                        !getline(asm_file, sound_len_str)) {
                        GtkWidget *message_label = GTK_WIDGET(g_object_get_data(G_OBJECT(text_view), "message-status-label"));
                        if (message_label != NULL) {
                            gtk_label_set_text(GTK_LABEL(message_label), "Invalid Asm8 file: incomplete sound data");
                        }
                        asm_file.close();
                        g_free(filename);
                        gtk_widget_destroy(dialog);
                        return;
                    }
                    
                    uint32_t sound_addr = std::stoul(sound_addr_str);
                    uint32_t sound_len = std::stoul(sound_len_str);
                    
                    applyCode.soundPath.push_back(sound_path);
                    applyCode.soundAddress.push_back(sound_addr);
                    applyCode.soundLength.push_back(sound_len);
                }

                // Read music count and data
                uint32_t music_count = 0;
                if (!parse_next_uint32(music_count)) {
                    GtkWidget *message_label = GTK_WIDGET(g_object_get_data(G_OBJECT(text_view), "message-status-label"));
                    if (message_label != NULL) {
                        gtk_label_set_text(GTK_LABEL(message_label), "Invalid Asm8 file: bad music count");
                    }
                    asm_file.close();
                    g_free(filename);
                    gtk_widget_destroy(dialog);
                    return;
                }

                for (uint32_t i = 0; i < music_count; i++) {
                    string music_path, music_addr_str, music_len_str;
                    if (!getline(asm_file, music_path) ||
                        !getline(asm_file, music_addr_str) ||
                        !getline(asm_file, music_len_str)) {
                        GtkWidget *message_label = GTK_WIDGET(g_object_get_data(G_OBJECT(text_view), "message-status-label"));
                        if (message_label != NULL) {
                            gtk_label_set_text(GTK_LABEL(message_label), "Invalid Asm8 file: incomplete music data");
                        }
                        asm_file.close();
                        g_free(filename);
                        gtk_widget_destroy(dialog);
                        return;
                    }
                    
                    uint32_t music_addr = std::stoul(music_addr_str);
                    uint32_t music_len = std::stoul(music_len_str);
                    
                    applyCode.musicPath.push_back(music_path);
                    applyCode.musicAddress.push_back(music_addr);
                    applyCode.musicLength.push_back(music_len);
                }

                if (GTK_IS_TEXT_VIEW(text_view)) {
                    GtkTextBuffer *text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));

                    if (text_buffer != NULL) {
                        GtkTextIter start_iter;
                        GtkTextIter end_iter;
                        gtk_text_buffer_get_start_iter(text_buffer, &start_iter);
                        gtk_text_buffer_get_end_iter(text_buffer, &end_iter);
                        gtk_text_buffer_delete(text_buffer, &start_iter, &end_iter);

                        string line;
                        bool first_content_line = true;
                        const char *newline = "\n";
                        std::string rebuilt_text;
                        while (getline(asm_file, line)) {
                            if (!first_content_line) {
                                gtk_text_buffer_insert(text_buffer, &start_iter, newline, -1);
                                rebuilt_text += '\n';
                            }
                            gtk_text_buffer_insert(text_buffer, &start_iter, line.c_str(), -1);
                            rebuilt_text += line;
                            gtk_text_buffer_get_end_iter(text_buffer, &start_iter);
                            first_content_line = false;
                        }

                        g_undo_stack.clear();
                        g_redo_stack.clear();
                        g_current_text = rebuilt_text;

                        apply_editor_highlighting(text_buffer);

                        gtk_text_buffer_get_start_iter(text_buffer, &start_iter);
                        gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(text_view), &start_iter, 0.0, FALSE, 0.0, 0.0);

                        applyCode.asmFile = filename;

                        GtkWidget *message_label = GTK_WIDGET(g_object_get_data(G_OBJECT(text_view), "message-status-label"));
                        if (message_label != NULL) {
                            gtk_label_set_text(GTK_LABEL(message_label), "Asm8 file loaded");
                        }
                    }
                }
            }

            asm_file.close();
            g_free(filename);
        }
    }

    gtk_widget_destroy(dialog);
}

// save functions
void SaveProject(std::string filename) {
    if (filename.empty()) {
        return;
    }

    GtkWidget *text_view = main_text_view;
    GtkWidget *message_label = NULL;
    GtkTextBuffer *text_buffer = NULL;

    if (GTK_IS_TEXT_VIEW(text_view)) {
        text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
        message_label = GTK_WIDGET(g_object_get_data(G_OBJECT(text_view), "message-status-label"));
    }

    if (text_buffer == NULL) {
        if (message_label != NULL) {
            gtk_label_set_text(GTK_LABEL(message_label), "No editor buffer to save");
        }
        return;
    }

    // récupérer le nom du fichier ASM et changer l'extension en ".binm8"
    applyCode.binFile = applyCode.asmFile.substr(0, applyCode.asmFile.find_last_of('.')) + ".binm8";

    GtkTextIter start_iter;
    GtkTextIter end_iter;
    gtk_text_buffer_get_start_iter(text_buffer, &start_iter);
    gtk_text_buffer_get_end_iter(text_buffer, &end_iter);

    gchar *raw_text = gtk_text_buffer_get_text(text_buffer, &start_iter, &end_iter, FALSE);
    std::string editor_text = raw_text != NULL ? raw_text : "";
    g_free(raw_text);

    ofstream project_file(filename, ios::binary | ios::trunc);
    if (!project_file.is_open()) {
        if (message_label != NULL) {
            gtk_label_set_text(GTK_LABEL(message_label), "Failed to open file for saving");
        }
        return;
    }

    project_file << applyCode.binFile << '\n'
                 << applyCode.spritesheetCount << '\n'
                 << applyCode.tilesetCount << '\n'
                 << applyCode.soundCount << '\n'
                 << applyCode.musicCount << '\n';

    // sauvegarder les sprites dans le fichier projet
    for(uint32_t i = 0; i < applyCode.spritesheetCount; i++) {
        project_file << applyCode.spritesheetPath[i] << '\n'
                     << applyCode.spritesheetAddress[i] << '\n'
                     << applyCode.spritesheetLength[i] << '\n'
                     << applyCode.spritesheetWidth[i] << '\n'
                     << applyCode.spritesheetHeight[i] << '\n'
                     << applyCode.spriteCount[i] << '\n';

        for(uint32_t j = 0; j < applyCode.spriteCount[i]; j++) {
            project_file << applyCode.spriteAddress[i][j] << '\n'
                         << applyCode.spriteLength[i][j] << '\n'
                         << applyCode.spriteWidth[i][j] << '\n'
                         << applyCode.spriteHeight[i][j] << '\n';
        }
    }

    // sauvegarder les tiles dans le fichier projet
    for(uint32_t i = 0; i < applyCode.tilesetCount; i++) {
        project_file << applyCode.tilesetPath[i] << '\n'
                     << applyCode.tilesetAddress[i] << '\n'
                     << applyCode.tilesetLength[i] << '\n'
                     << applyCode.tilesetWidth[i] << '\n'
                     << applyCode.tilesetHeight[i] << '\n'
                     << applyCode.tileCount[i] << '\n';

        for(uint32_t j = 0; j < applyCode.tileCount[i]; j++) {
            project_file << applyCode.tileAddress[i][j] << '\n'
                         << applyCode.tileLength[i][j] << '\n'
                         << applyCode.tileWidth[i][j] << '\n'
                         << applyCode.tileHeight[i][j] << '\n';
        }
    }

    project_file << applyCode.soundCount << '\n';
    
    for(uint32_t i = 0; i < applyCode.soundCount; i++) {
        project_file << applyCode.soundPath[i] << '\n'
                     << applyCode.soundAddress[i] << '\n'
                     << applyCode.soundLength[i] << '\n';
    }

    project_file << applyCode.musicCount << '\n';
    
    for(uint32_t i = 0; i < applyCode.musicCount; i++) {
        project_file << applyCode.musicPath[i] << '\n'
                     << applyCode.musicAddress[i] << '\n'
                     << applyCode.musicLength[i] << '\n';
    }

    project_file << editor_text;
    if (!editor_text.empty() && editor_text.back() != '\n') {
        project_file << '\n';
    }

    // Vérifier l'état du stream AVANT d'appeler apply_editor_highlighting
    if (!project_file) {
        if (message_label != NULL) {
            gtk_label_set_text(GTK_LABEL(message_label), "Failed to write project data");
        }
        project_file.close();
        return;
    }

    project_file.flush();
    if (!project_file) {
        if (message_label != NULL) {
            gtk_label_set_text(GTK_LABEL(message_label), "Failed to flush project data");
        }
        project_file.close();
        return;
    }

    project_file.close();
    if (project_file.fail()) {
        if (message_label != NULL) {
            gtk_label_set_text(GTK_LABEL(message_label), "Failed to finalize saved file");
        }
        return;
    }

    if (message_label != NULL) {
        gtk_label_set_text(GTK_LABEL(message_label), "Project saved");
    }

    // Appliquer la coloration syntaxique une seule fois à la fin
    apply_editor_highlighting(text_buffer);
}

std::string SaveProjectAs(GtkWindow *parent_window) {
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Save Asm8 Project",
                                                    parent_window,
                                                    GTK_FILE_CHOOSER_ACTION_SAVE,
                                                    "_Cancel", GTK_RESPONSE_CANCEL,
                                                    "_Save", GTK_RESPONSE_ACCEPT,
                                                    NULL);

    GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);

    gtk_file_chooser_set_do_overwrite_confirmation(chooser, TRUE);
    gtk_file_chooser_set_current_name(chooser, "Untitled.asm8");

    if (!applyCode.asmFile.empty()) {
        gtk_file_chooser_set_filename(chooser, applyCode.asmFile.c_str());
    }

    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "Asm8 files (*.asm8)");
    gtk_file_filter_add_pattern(filter, "*.asm8");
    gtk_file_chooser_add_filter(chooser, filter);

    std::string selected_path;

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(chooser);

        if (filename != NULL) {
            selected_path = filename;
            g_free(filename);
        }
    }

    gtk_widget_destroy(dialog);

    if (!selected_path.empty()) {
        applyCode.asmFile = selected_path;
    }

    return selected_path;
}

// load function
void LoadROM(std::string filename) {
    if (filename.empty()) {
        return;
    }

    GtkWidget *text_view = main_text_view;
    GtkWidget *message_label = NULL;
    GtkTextBuffer *text_buffer = NULL;

    if (GTK_IS_TEXT_VIEW(text_view)) {
        text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
        message_label = GTK_WIDGET(g_object_get_data(G_OBJECT(text_view), "message-status-label"));
    }

    if (text_buffer == NULL) {
        if (message_label != NULL) {
            gtk_label_set_text(GTK_LABEL(message_label), "No editor buffer available");
        }
        return;
    }

    constexpr std::size_t rom_start_offset = 0x400;
    constexpr std::size_t rom_end_offset = 0x5000000; // exclusive upper bound

    if (rom_end_offset > sizeof(applyCode.memory) || rom_start_offset >= rom_end_offset) {
        if (message_label != NULL) {
            gtk_label_set_text(GTK_LABEL(message_label), "Invalid memory range for ROM import");
        }
        return;
    }

    const std::size_t rom_size = rom_end_offset - rom_start_offset;
    std::ifstream rom_file(filename, std::ios::binary);
    if (!rom_file.is_open()) {
        if (message_label != NULL) {
            gtk_label_set_text(GTK_LABEL(message_label), "Failed to load file");
        }
        return;
    }

    // Reset target buffer area before loading new data
    std::memset(applyCode.memory + rom_start_offset, 0, rom_size);

    rom_file.read(reinterpret_cast<char *>(applyCode.memory + rom_start_offset), static_cast<std::streamsize>(rom_size));

    if (rom_file.bad()) {
        if (message_label != NULL) {
            gtk_label_set_text(GTK_LABEL(message_label), "Failed to read ROM data");
        }
        rom_file.close();
        return;
    }

    const std::streamsize bytes_read = rom_file.gcount();
    if (bytes_read == 0) {
        if (message_label != NULL) {
            gtk_label_set_text(GTK_LABEL(message_label), "ROM file is empty");
        }
        rom_file.close();
        return;
    }

    rom_file.close();

    if (message_label != NULL) {
        gtk_label_set_text(GTK_LABEL(message_label), "ROM imported");
    }
}

// save function
void SaveROM(std::string filename) {
    if (filename.empty()) {
        return;
    }

    GtkWidget *text_view = main_text_view;
    GtkWidget *message_label = NULL;
    GtkTextBuffer *text_buffer = NULL;

    if (GTK_IS_TEXT_VIEW(text_view)) {
        text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
        message_label = GTK_WIDGET(g_object_get_data(G_OBJECT(text_view), "message-status-label"));
    }

    if (text_buffer == NULL) {
        if (message_label != NULL) {
            gtk_label_set_text(GTK_LABEL(message_label), "No editor buffer to save");
        }
        return;
    }

    GtkTextIter start_iter;
    GtkTextIter end_iter;
    gtk_text_buffer_get_start_iter(text_buffer, &start_iter);
    gtk_text_buffer_get_end_iter(text_buffer, &end_iter);

    gchar *raw_text = gtk_text_buffer_get_text(text_buffer, &start_iter, &end_iter, FALSE);
    std::string editor_text = raw_text != NULL ? raw_text : "";
    g_free(raw_text);

    ofstream rom_file(filename, ios::binary | ios::trunc);
    if (!rom_file.is_open()) {
        if (message_label != NULL) {
            gtk_label_set_text(GTK_LABEL(message_label), "Failed to open file for saving");
        }
        return;
    }

    constexpr std::size_t rom_start_offset = 0x400;
    constexpr std::size_t rom_end_offset = 0x5000000; // exclusive upper bound

    if (rom_end_offset > sizeof(applyCode.memory) || rom_start_offset >= rom_end_offset) {
        if (message_label != NULL) {
            gtk_label_set_text(GTK_LABEL(message_label), "Invalid memory range for ROM export");
        }
        rom_file.close();
        return;
    }

    rom_file.write(reinterpret_cast<const char *>(applyCode.memory + rom_start_offset),
                   rom_end_offset - rom_start_offset);

    if (!rom_file) {
        if (message_label != NULL) {
            gtk_label_set_text(GTK_LABEL(message_label), "Failed to write ROM data");
        }
        rom_file.close();
        return;
    }

    rom_file.flush();
    if (!rom_file) {
        if (message_label != NULL) {
            gtk_label_set_text(GTK_LABEL(message_label), "Failed to flush ROM data");
        }
        rom_file.close();
        return;
    }

    rom_file.close();
    if (rom_file.fail()) {
        if (message_label != NULL) {
            gtk_label_set_text(GTK_LABEL(message_label), "Failed to finalize exported file");
        }
        return;
    }

    applyCode.binFile = filename;

    if (message_label != NULL) {
        gtk_label_set_text(GTK_LABEL(message_label), "ROM exported");
    }
}

bool Build(void) {
        // clear labels before compiling
    for(uint32_t i = 0; i < 0x1000000; i++) {
        applyCode.label[i][0] = "";
        applyCode.label[i][1] = "";
    }

    // 1st pass: find labels
    applyCode.pass = 1;
    applyCode.PC = 0x400;

    // get number of line in the texte editor
    GtkWidget *text_view = main_text_view;
    GtkTextBuffer *buffer = NULL;

    if (GTK_IS_TEXT_VIEW(text_view)) {
        buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    }

    if (buffer == NULL) {
        GtkWidget *message_label = GTK_WIDGET(g_object_get_data(G_OBJECT(text_view), "message-status-label"));
        if (message_label != NULL) {
            gtk_label_set_text(GTK_LABEL(message_label), "No editor buffer to compile");
        }
        return false;
    }
    gint buffer_line_count = gtk_text_buffer_get_line_count(buffer);

    std::string err = "";

    for(uint32_t i = 0; i < (uint32_t)buffer_line_count; i++) {
        // get the line of text from the text view buffer at indice i
        GtkTextIter line_start;
        GtkTextIter line_end;
        gtk_text_buffer_get_iter_at_line(buffer, &line_start, i);
        line_end = line_start;
        gtk_text_iter_forward_to_line_end(&line_end);

        gchar *raw_line = gtk_text_buffer_get_text(buffer, &line_start, &line_end, FALSE);
        std::string line = "";

        if(raw_line != NULL) {
            line = raw_line;

            Trim(line);
        }
        g_free(raw_line);

        if(line != "") {
            err = applyCode.CompileLine(line, i + 1);

            if(err != "") {
                GtkWidget *message_label = GTK_WIDGET(g_object_get_data(G_OBJECT(text_view), "message-status-label"));
                if (message_label != NULL) {
                    gtk_label_set_text(GTK_LABEL(message_label), err.c_str());
                }
                break;
            }
        }
    }

    if(err == "") {
        // 2nd pass
        applyCode.pass = 2;        
        applyCode.lastLabelLine = applyCode.PC;
        applyCode.PC = 0x400;

        for(uint32_t i = 0; i < (uint32_t)buffer_line_count; i++) {
            // get the line of text from the text view buffer at indice i
            GtkTextIter line_start;
            GtkTextIter line_end;
            gtk_text_buffer_get_iter_at_line(buffer, &line_start, i);
            line_end = line_start;
            gtk_text_iter_forward_to_line_end(&line_end);

            gchar *raw_line = gtk_text_buffer_get_text(buffer, &line_start, &line_end, FALSE);
            std::string line = "";

            if(raw_line != NULL) {
                line = raw_line;

                Trim(line);
            }
            g_free(raw_line);

            if(line != "") {
                err = applyCode.CompileLine(line, i + 1);

                if(err != "") {
                    GtkWidget *message_label = GTK_WIDGET(g_object_get_data(G_OBJECT(text_view), "message-status-label"));
                    if (message_label != NULL) {
                        gtk_label_set_text(GTK_LABEL(message_label), err.c_str());
                    }
                    break;
                }
            }
        }

        if(err == "") {
            GtkWidget *message_label = GTK_WIDGET(g_object_get_data(G_OBJECT(text_view), "message-status-label"));
            if (message_label != NULL) {
                gtk_label_set_text(GTK_LABEL(message_label), "Program compiled");
            }            
        }

        if(err == "") { return true; }
    }

    return false;
}

std::string SaveROMAs(GtkWindow *parent_window) {
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Save binm8 ROM",
                                                    parent_window,
                                                    GTK_FILE_CHOOSER_ACTION_SAVE,
                                                    "_Cancel", GTK_RESPONSE_CANCEL,
                                                    "_Save", GTK_RESPONSE_ACCEPT,
                                                    NULL);

    GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);

    gtk_file_chooser_set_do_overwrite_confirmation(chooser, TRUE);
    gtk_file_chooser_set_current_name(chooser, "Untitled.binm8");

    if (!applyCode.binFile.empty()) {
        gtk_file_chooser_set_filename(chooser, applyCode.binFile.c_str());
    }

    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "binm8 ROM files (*.binm8)");
    gtk_file_filter_add_pattern(filter, "*.binm8");
    gtk_file_chooser_add_filter(chooser, filter);

    std::string selected_path;

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(chooser);

        if (filename != NULL) {
            selected_path = filename;
            g_free(filename);
        }
    }

    gtk_widget_destroy(dialog);

    if (!selected_path.empty()) {
        applyCode.binFile = selected_path;
    }

    return selected_path;
}
