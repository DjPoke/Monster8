#include "gtk.hpp"
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

using namespace std;



// prompt for frame dimensions
bool prompt_for_frame_dimensions(GtkWindow *parent_window, gint image_width, gint image_height, gint &frame_width, gint &frame_height) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Frame Dimensions",
                                                    parent_window,
                                                    GTK_DIALOG_MODAL,
                                                    "_Cancel",
                                                    GTK_RESPONSE_CANCEL,
                                                    "_OK",
                                                    GTK_RESPONSE_OK,
                                                    NULL);

    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_box_pack_start(GTK_BOX(container), gtk_label_new("Enter frame dimensions (pixels):"), FALSE, FALSE, 0);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 6);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 6);

    GtkWidget *width_label = gtk_label_new("Frame width:");
    gtk_widget_set_halign(width_label, GTK_ALIGN_START);
    GtkWidget *height_label = gtk_label_new("Frame height:");
    gtk_widget_set_halign(height_label, GTK_ALIGN_START);

    GtkWidget *width_entry = gtk_entry_new();
    GtkWidget *height_entry = gtk_entry_new();

    std::string width_text = std::to_string(frame_width);
    std::string height_text = std::to_string(frame_height);
    gtk_entry_set_text(GTK_ENTRY(width_entry), width_text.c_str());
    gtk_entry_set_text(GTK_ENTRY(height_entry), height_text.c_str());

    gtk_entry_set_activates_default(GTK_ENTRY(width_entry), TRUE);
    gtk_entry_set_activates_default(GTK_ENTRY(height_entry), TRUE);

    gtk_grid_attach(GTK_GRID(grid), width_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), width_entry, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), height_label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), height_entry, 1, 1, 1, 1);

    GtkWidget *error_label = gtk_label_new("");
    gtk_widget_set_halign(error_label, GTK_ALIGN_START);

    gtk_box_pack_start(GTK_BOX(container), grid, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(container), error_label, FALSE, FALSE, 0);

    gtk_container_add(GTK_CONTAINER(content_area), container);
    gtk_widget_show_all(dialog);

    auto parse_positive_int = [](const gchar *text, gint &out_value) -> bool {
        if (text == NULL) {
            return false;
        }
        errno = 0;
        char *endptr = NULL;
        long parsed = std::strtol(text, &endptr, 10);
        if (errno != 0 || endptr == text || (endptr != NULL && *endptr != '\0')) {
            return false;
        }
        if (parsed <= 0 || parsed > G_MAXINT) {
            return false;
        }
        out_value = static_cast<gint>(parsed);
        return true;
    };

    bool success = false;

    while (!success) {
        gint response = gtk_dialog_run(GTK_DIALOG(dialog));
        if (response != GTK_RESPONSE_OK) {
            gtk_widget_destroy(dialog);
            return false;
        }

        gint new_width = 0;
        gint new_height = 0;
        bool width_valid = parse_positive_int(gtk_entry_get_text(GTK_ENTRY(width_entry)), new_width);
        bool height_valid = parse_positive_int(gtk_entry_get_text(GTK_ENTRY(height_entry)), new_height);

        if (!width_valid || !height_valid) {
            gtk_label_set_text(GTK_LABEL(error_label), "Please enter valid positive integers.");
            continue;
        }

        frame_width = new_width;
        frame_height = new_height;
        success = true;
    }

    gtk_widget_destroy(dialog);
    return true;
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

    GtkTextIter start_iter;
    GtkTextIter end_iter;
    gtk_text_buffer_get_start_iter(text_buffer, &start_iter);
    gtk_text_buffer_get_end_iter(text_buffer, &end_iter);

    const gint buffer_line_count = gtk_text_buffer_get_line_count(text_buffer);

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
                 << applyCode.spriteCount << '\n'
                 << applyCode.tilesetCount << '\n'
                 << applyCode.soundCount << '\n'
                 << applyCode.musicCount << '\n'
                 << applyCode.nextSpriteAddr << '\n'
                 << applyCode.nextTilesetAddr << '\n'
                 << applyCode.nextSoundAddr << '\n'
                 << applyCode.nextMusicAddr << '\n';

    project_file << applyCode.spritePath.size() << '\n';

    for(int i = 0; i < applyCode.spritePath.size(); i++) {
        project_file << applyCode.spritePath[i] << '\n'
                     << applyCode.spriteAddress[i] << '\n'
                     << applyCode.spriteLength[i] << '\n'
                     << applyCode.spriteWidth[i] << '\n'
                     << applyCode.spriteHeight[i] << '\n';
    }

    project_file << applyCode.tilesetPath.size() << '\n';
    
    for(int i = 0; i < applyCode.tilesetPath.size(); i++) {
        project_file << applyCode.tilesetPath[i] << '\n'
                     << applyCode.tilesetAddress[i] << '\n'
                     << applyCode.tilesetLength[i] << '\n';
    }

    project_file << applyCode.soundPath.size() << '\n';
    
    for(int i = 0; i < applyCode.soundPath.size(); i++) {
        project_file << applyCode.soundPath[i] << '\n'
                     << applyCode.soundAddress[i] << '\n'
                     << applyCode.soundLength[i] << '\n';
    }

    project_file << applyCode.musicPath.size() << '\n';
    
    for(int i = 0; i < applyCode.musicPath.size(); i++) {
        project_file << applyCode.musicPath[i] << '\n'
                     << applyCode.musicAddress[i] << '\n'
                     << applyCode.musicLength[i] << '\n';
    }

    project_file << static_cast<int>(buffer_line_count) << '\n';

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
        // Convertir le chemin absolu en chemin relatif
        try {
            std::filesystem::path abs_path(selected_path);
            std::filesystem::path current_dir = std::filesystem::current_path();
            std::filesystem::path relative_path = std::filesystem::relative(abs_path, current_dir);
            selected_path = relative_path.string();
        } catch (const std::filesystem::filesystem_error& ex) {
            // En cas d'erreur, garder le chemin original
            std::cout << "Erreur lors de la conversion du chemin: " << ex.what() << std::endl;
        }

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

    const gint buffer_line_count = gtk_text_buffer_get_line_count(text_buffer);

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

    for(uint32_t i = 0; i < buffer_line_count; i++) {
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

        for(uint32_t i = 0; i < buffer_line_count; i++) {
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

void ensure_highlight_tags(GtkTextBuffer *buffer) {
    if (buffer == NULL) {
        return;
    }

    GtkTextTagTable *tag_table = gtk_text_buffer_get_tag_table(buffer);
    if (tag_table == NULL) {
        return;
    }

    if (gtk_text_tag_table_lookup(tag_table, "label") == NULL) {
        gtk_text_buffer_create_tag(buffer,
                                   "label",
                                   "foreground", "#1f6feb",
                                   NULL);
    }

    if (gtk_text_tag_table_lookup(tag_table, "value") == NULL) {
        gtk_text_buffer_create_tag(buffer,
                                   "value",
                                   "foreground", "#2ecc71",
                                   NULL);
    }

    if (gtk_text_tag_table_lookup(tag_table, "comment") == NULL) {
        gtk_text_buffer_create_tag(buffer,
                                   "comment",
                                   "foreground", "#46a546",
                                   NULL);
    }
}

void apply_editor_highlighting(GtkTextBuffer *buffer) {
    if (buffer == NULL) {
        return;
    }

    ensure_highlight_tags(buffer);

    GtkTextIter start_iter;
    GtkTextIter end_iter;
    gtk_text_buffer_get_start_iter(buffer, &start_iter);
    gtk_text_buffer_get_end_iter(buffer, &end_iter);
    gtk_text_buffer_remove_all_tags(buffer, &start_iter, &end_iter);

    const gint line_count = gtk_text_buffer_get_line_count(buffer);

    for (gint line_index = 0; line_index < line_count; ++line_index) {
        GtkTextIter line_start;
        GtkTextIter line_end;
        gtk_text_buffer_get_iter_at_line(buffer, &line_start, line_index);
        line_end = line_start;
        gtk_text_iter_forward_to_line_end(&line_end);

        if (gtk_text_iter_equal(&line_start, &line_end)) {
            continue;
        }

        gchar *line_text = gtk_text_buffer_get_text(buffer, &line_start, &line_end, FALSE);
        if (line_text == NULL) {
            continue;
        }

        std::string line(line_text);
        g_free(line_text);

        const std::size_t comment_pos = line.find(';');
        if (comment_pos != std::string::npos) {
            GtkTextIter comment_start = line_start;
            gtk_text_iter_forward_chars(&comment_start, static_cast<int>(comment_pos));
            GtkTextIter comment_end = line_end;
            gtk_text_buffer_apply_tag_by_name(buffer, "comment", &comment_start, &comment_end);
        }

        const std::size_t label_pos = line.find(':');
        if (label_pos != std::string::npos &&
            (comment_pos == std::string::npos || label_pos < comment_pos)) {
            std::size_t label_start_index = 0;
            while (label_start_index < label_pos &&
                   std::isspace(static_cast<unsigned char>(line[label_start_index]))) {
                label_start_index++;
            }

            if (label_start_index < label_pos) {
                GtkTextIter label_start = line_start;
                GtkTextIter label_end = line_start;
                gtk_text_iter_forward_chars(&label_start, static_cast<int>(label_start_index));
                gtk_text_iter_forward_chars(&label_end, static_cast<int>(label_pos));
                gtk_text_buffer_apply_tag_by_name(buffer, "label", &label_start, &label_end);
            }
        }

        const std::size_t scan_limit = (comment_pos != std::string::npos)
                                           ? comment_pos
                                           : line.length();

        const auto is_separator = [](char ch) -> bool {
            return std::isspace(static_cast<unsigned char>(ch)) ||
                   ch == ',' || ch == '[' || ch == ']' || ch == '(' || ch == ')' ||
                   ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '=' ||
                   ch == ':';
        };

        std::size_t index = 0;
        while (index < scan_limit) {
            char current = line[index];
            char previous = (index == 0) ? '\0' : line[index - 1];
            const bool allowed_start = (index == 0) || is_separator(previous);

            if (!allowed_start) {
                index++;
                continue;
            }

            std::size_t value_start_index = index;
            bool is_number = false;

            if (current == '-' && (index + 1) < scan_limit) {
                char next = line[index + 1];
                if (std::isdigit(static_cast<unsigned char>(next))) {
                    is_number = true;
                    index += 2;
                    while (index < scan_limit &&
                           std::isdigit(static_cast<unsigned char>(line[index]))) {
                        index++;
                    }
                } else if (next == '0' && (index + 2) < scan_limit &&
                           (line[index + 2] == 'x' || line[index + 2] == 'X')) {
                    std::size_t hex_index = index + 3;
                    bool has_digits = false;
                    while (hex_index < scan_limit &&
                           std::isxdigit(static_cast<unsigned char>(line[hex_index]))) {
                        hex_index++;
                        has_digits = true;
                    }
                    if (has_digits) {
                        is_number = true;
                    }
                    index = hex_index;
                }
            } else if (current == '0' && (index + 1) < scan_limit &&
                       (line[index + 1] == 'x' || line[index + 1] == 'X')) {
                std::size_t hex_index = index + 2;
                bool has_digits = false;
                while (hex_index < scan_limit &&
                       std::isxdigit(static_cast<unsigned char>(line[hex_index]))) {
                    hex_index++;
                    has_digits = true;
                }
                if (has_digits) {
                    is_number = true;
                }
                index = hex_index;
            } else if (std::isdigit(static_cast<unsigned char>(current))) {
                is_number = true;
                index++;
                while (index < scan_limit &&
                       std::isdigit(static_cast<unsigned char>(line[index]))) {
                    index++;
                }
            } else {
                index++;
            }

            if (is_number) {
                GtkTextIter value_start = line_start;
                GtkTextIter value_end = line_start;
                gtk_text_iter_forward_chars(&value_start, static_cast<int>(value_start_index));
                gtk_text_iter_forward_chars(&value_end, static_cast<int>(index));

                if (!gtk_text_iter_equal(&value_start, &value_end)) {
                    gtk_text_buffer_apply_tag_by_name(buffer, "value", &value_start, &value_end);
                }
            }
        }
    }
}

void load_editor_css(GtkWidget *text_view) {
    if (text_view == NULL) {
        return;
    }

    GtkCssProvider *provider = gtk_css_provider_new();
    if (provider == NULL) {
        return;
    }

    GError *error = NULL;
    const gchar *css_path = "textview.css";

    gtk_css_provider_load_from_path(provider, css_path, &error);

    if (error != NULL) {
        g_warning("Failed to load CSS file '%s': %s", css_path, error->message);
        g_error_free(error);
        g_object_unref(provider);
        return;
    }

    GtkStyleContext *context = gtk_widget_get_style_context(text_view);
    gtk_style_context_add_provider(context,
                                   GTK_STYLE_PROVIDER(provider),
                                   GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    GtkWidget *parent = gtk_widget_get_parent(text_view);
    if (parent != NULL) {
        GtkStyleContext *parent_context = gtk_widget_get_style_context(parent);
        gtk_style_context_add_provider(parent_context,
                                       GTK_STYLE_PROVIDER(provider),
                                       GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    }

    GdkScreen *screen = gtk_widget_get_screen(text_view);
    if (screen != NULL) {
        gtk_style_context_add_provider_for_screen(screen,
                                                  GTK_STYLE_PROVIDER(provider),
                                                  GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    }

    gtk_widget_queue_draw(text_view);
    g_object_unref(provider);
}

void on_buffer_changed(GtkTextBuffer *buffer, gpointer user_data) {
    GtkWidget *message_label = GTK_WIDGET(user_data);

    if (g_is_applying_history) {
        return; // ignore notifications while restoring
    }

    if (buffer == NULL) {
        return;
    }

    GtkTextIter start_iter;
    GtkTextIter end_iter;
    gtk_text_buffer_get_start_iter(buffer, &start_iter);
    gtk_text_buffer_get_end_iter(buffer, &end_iter);

    gchar *raw_text = gtk_text_buffer_get_text(buffer, &start_iter, &end_iter, FALSE);
    std::string new_text = raw_text != NULL ? raw_text : "";
    g_free(raw_text);

    if (new_text == g_current_text) {
        return;
    }

    if (!g_current_text.empty() || !new_text.empty()) {
        g_undo_stack.push_back(g_current_text);
        if (g_undo_stack.size() > g_max_history_entries) {
            g_undo_stack.erase(g_undo_stack.begin());
        }
        g_redo_stack.clear();
    }

    g_current_text = new_text;

    apply_editor_highlighting(buffer);

    if (message_label != NULL) {
        char status[64];
        g_snprintf(status, sizeof(status), "Undo: %zu", g_undo_stack.size());
        gtk_label_set_text(GTK_LABEL(message_label), status);
    }
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
        // Convertir le chemin absolu en chemin relatif
        try {
            std::filesystem::path abs_path(selected_path);
            std::filesystem::path current_dir = std::filesystem::current_path();
            std::filesystem::path relative_path = std::filesystem::relative(abs_path, current_dir);
            selected_path = relative_path.string();
        } catch (const std::filesystem::filesystem_error& ex) {
            // En cas d'erreur, garder le chemin original
            std::cout << "Erreur lors de la conversion du chemin: " << ex.what() << std::endl;
        }

        applyCode.binFile = selected_path;
    }

    return selected_path;
}

// editor's functions
void
on_project_new_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    (void)menuitem;
    GtkWidget *text_view = GTK_WIDGET(user_data);
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));

    // Clear undo/redo stacks for a fresh document
    g_undo_stack.clear();
    g_redo_stack.clear();
    g_current_text.clear();

    gtk_text_buffer_set_text(buffer, "", -1);
    apply_editor_highlighting(buffer);

    for (uint32_t i = 0; i < 0x5000000; i++) {
        applyCode.memory[i] = 0x00;

        uint32_t j = i % 0x10000;

        if (i <= 0xffffff) {
            applyCode.label[i][0] = "";
            applyCode.label[i][1] = "";
        }
    }
                      
    applyCode.asmFile = "";
    applyCode.binFile = "";

    applyCode.spriteCount = 0;
    applyCode.tilesetCount = 0;
    applyCode.soundCount = 0;
    applyCode.musicCount = 0;

    applyCode.nextSpriteAddr = 0x000000;
    applyCode.nextTilesetAddr = 0x000000;
    applyCode.nextSoundAddr = 0x000000;
    applyCode.nextMusicAddr = 0x000000;

    applyCode.spriteCount = 0;
    applyCode.tilesetCount = 0;
    applyCode.soundCount = 0;
    applyCode.musicCount = 0;

    applyCode.spritePath.clear();
    applyCode.spriteAddress.clear();
    applyCode.spriteLength.clear();
    applyCode.spriteWidth.clear();
    applyCode.spriteHeight.clear();
    applyCode.tilesetPath.clear();
    applyCode.tilesetAddress.clear();
    applyCode.tilesetLength.clear();
    applyCode.soundPath.clear();
    applyCode.soundAddress.clear();
    applyCode.soundLength.clear();
    applyCode.musicPath.clear();
    applyCode.musicAddress.clear();
    applyCode.musicLength.clear();
            
    GtkWidget *message_label = GTK_WIDGET(g_object_get_data(G_OBJECT(text_view), "message-status-label"));

    if (message_label != NULL) {
        gtk_label_set_text(GTK_LABEL(message_label), "Project cleared");
    }
}

void
on_project_open_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    (void)menuitem;
    GtkWidget *text_view = GTK_WIDGET(user_data);
    GtkWindow *parent_window = NULL;

    if (text_view != NULL) {
        GtkWidget *toplevel = gtk_widget_get_toplevel(text_view);
        if (GTK_IS_WINDOW(toplevel)) {
            parent_window = GTK_WINDOW(toplevel);
        }
    }

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
                applyCode.spritePath.clear();
                applyCode.spriteAddress.clear();
                applyCode.spriteLength.clear();
                applyCode.spriteWidth.clear();
                applyCode.spriteHeight.clear();
                applyCode.tilesetPath.clear();
                applyCode.tilesetAddress.clear();
                applyCode.tilesetLength.clear();
                applyCode.tilesetWidth.clear();
                applyCode.tilesetHeight.clear();
                applyCode.soundPath.clear();
                applyCode.soundAddress.clear();
                applyCode.soundLength.clear();
                applyCode.musicPath.clear();
                applyCode.musicAddress.clear();
                applyCode.musicLength.clear();

                if (!parse_next_uint32(applyCode.spriteCount) ||
                    !parse_next_uint32(applyCode.tilesetCount) ||
                    !parse_next_uint32(applyCode.soundCount) ||
                    !parse_next_uint32(applyCode.musicCount) ||
                    !parse_next_uint32(applyCode.nextSpriteAddr) ||
                    !parse_next_uint32(applyCode.nextTilesetAddr) ||
                    !parse_next_uint32(applyCode.nextSoundAddr) ||
                    !parse_next_uint32(applyCode.nextMusicAddr)) {
                    GtkWidget *message_label = GTK_WIDGET(g_object_get_data(G_OBJECT(text_view), "message-status-label"));
                    if (message_label != NULL) {
                        gtk_label_set_text(GTK_LABEL(message_label), "Invalid Asm8 file: bad header values");
                    }
                    asm_file.close();
                    g_free(filename);
                    gtk_widget_destroy(dialog);
                    return;
                }

                // Read sprite data
                uint32_t sprite_vector_size = 0;
                if (!parse_next_uint32(sprite_vector_size)) {
                    GtkWidget *message_label = GTK_WIDGET(g_object_get_data(G_OBJECT(text_view), "message-status-label"));
                    if (message_label != NULL) {
                        gtk_label_set_text(GTK_LABEL(message_label), "Invalid Asm8 file: bad sprite vector size");
                    }
                    asm_file.close();
                    g_free(filename);
                    gtk_widget_destroy(dialog);
                    return;
                }

                for (uint32_t i = 0; i < sprite_vector_size; i++) {
                    string sprite_path, sprite_addr_str, sprite_len_str, sprite_width_str, sprite_height_str;
                    if (!getline(asm_file, sprite_path) ||
                        !getline(asm_file, sprite_addr_str) ||
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
                    
                    uint32_t sprite_addr = std::stoul(sprite_addr_str);
                    uint32_t sprite_len = std::stoul(sprite_len_str);
                    uint32_t sprite_width = std::stoul(sprite_width_str);
                    uint32_t sprite_height = std::stoul(sprite_height_str);
                    
                    applyCode.spritePath.push_back(sprite_path);
                    applyCode.spriteAddress.push_back(sprite_addr);
                    applyCode.spriteLength.push_back(sprite_len);
                    applyCode.spriteWidth.push_back(sprite_width);
                    applyCode.spriteHeight.push_back(sprite_height);
                }

                // Read tileset data
                uint32_t tileset_vector_size = 0;
                if (!parse_next_uint32(tileset_vector_size)) {
                    GtkWidget *message_label = GTK_WIDGET(g_object_get_data(G_OBJECT(text_view), "message-status-label"));
                    if (message_label != NULL) {
                        gtk_label_set_text(GTK_LABEL(message_label), "Invalid Asm8 file: bad tileset vector size");
                    }
                    asm_file.close();
                    g_free(filename);
                    gtk_widget_destroy(dialog);
                    return;
                }

                for (uint32_t i = 0; i < tileset_vector_size; i++) {
                    string tileset_path, tileset_addr_str, tileset_len_str;
                    if (!getline(asm_file, tileset_path) ||
                        !getline(asm_file, tileset_addr_str) ||
                        !getline(asm_file, tileset_len_str)) {
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
                    
                    applyCode.tilesetPath.push_back(tileset_path);
                    applyCode.tilesetAddress.push_back(tileset_addr);
                    applyCode.tilesetLength.push_back(tileset_len);
                }

                // Read sound data
                uint32_t sound_vector_size = 0;
                if (!parse_next_uint32(sound_vector_size)) {
                    GtkWidget *message_label = GTK_WIDGET(g_object_get_data(G_OBJECT(text_view), "message-status-label"));
                    if (message_label != NULL) {
                        gtk_label_set_text(GTK_LABEL(message_label), "Invalid Asm8 file: bad sound vector size");
                    }
                    asm_file.close();
                    g_free(filename);
                    gtk_widget_destroy(dialog);
                    return;
                }

                for (uint32_t i = 0; i < sound_vector_size; i++) {
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

                // Read music data
                uint32_t music_vector_size = 0;
                if (!parse_next_uint32(music_vector_size)) {
                    GtkWidget *message_label = GTK_WIDGET(g_object_get_data(G_OBJECT(text_view), "message-status-label"));
                    if (message_label != NULL) {
                        gtk_label_set_text(GTK_LABEL(message_label), "Invalid Asm8 file: bad music vector size");
                    }
                    asm_file.close();
                    g_free(filename);
                    gtk_widget_destroy(dialog);
                    return;
                }

                for (uint32_t i = 0; i < music_vector_size; i++) {
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

                // Read buffer line count
                uint32_t saved_buffer_line_count = 0;
                if (!parse_next_uint32(saved_buffer_line_count)) {
                    GtkWidget *message_label = GTK_WIDGET(g_object_get_data(G_OBJECT(text_view), "message-status-label"));
                    if (message_label != NULL) {
                        gtk_label_set_text(GTK_LABEL(message_label), "Invalid Asm8 file: bad buffer line count");
                    }
                    asm_file.close();
                    g_free(filename);
                    gtk_widget_destroy(dialog);
                    return;
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
                        uint32_t lines_loaded = 0;
                        std::string rebuilt_text;
                        while (getline(asm_file, line)) {
                            if (saved_buffer_line_count != 0 && lines_loaded >= saved_buffer_line_count) {
                                break;
                            }
                            if (!first_content_line) {
                                gtk_text_buffer_insert(text_buffer, &start_iter, newline, -1);
                                rebuilt_text += '\n';
                            }
                            gtk_text_buffer_insert(text_buffer, &start_iter, line.c_str(), -1);
                            rebuilt_text += line;
                            gtk_text_buffer_get_end_iter(text_buffer, &start_iter);
                            first_content_line = false;
                            ++lines_loaded;
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

void
on_project_save_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    (void)menuitem;
    (void)user_data;

    GtkWidget *text_view = GTK_WIDGET(user_data);
    GtkWindow *parent_window = NULL;

    if (text_view != NULL) {
        GtkWidget *toplevel = gtk_widget_get_toplevel(text_view);
        if (GTK_IS_WINDOW(toplevel)) {
            parent_window = GTK_WINDOW(toplevel);
        }
    }

    if(applyCode.asmFile == "") {
        // save as
        applyCode.asmFile = SaveProjectAs(parent_window);

        // save file
        if(applyCode.asmFile != "") {
            SaveProject(applyCode.asmFile);

        } else {
            GtkWidget *message_label = GTK_WIDGET(g_object_get_data(G_OBJECT(text_view), "message-status-label"));
    
            if (message_label != NULL) {
                gtk_label_set_text(GTK_LABEL(message_label), "Save canceled");
            }
        }
    } else {
        // save file
        SaveProject(applyCode.asmFile);
    }
}

void
on_project_export_rom_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    (void)menuitem;
    (void)user_data;


    GtkWidget *text_view = GTK_WIDGET(user_data);
    GtkWindow *parent_window = NULL;

    if (text_view != NULL) {
        GtkWidget *toplevel = gtk_widget_get_toplevel(text_view);
        if (GTK_IS_WINDOW(toplevel)) {
            parent_window = GTK_WINDOW(toplevel);
        }
    }
    
    // save file requester asks for path to export rom
    // then save rom
    std::string selectedBin = SaveROMAs(parent_window);

    if (!selectedBin.empty()) {
        applyCode.binFile = selectedBin;
    }

    if (!applyCode.binFile.empty()) {
        if(!applyCode.asmFile.empty()) {
            SaveProject(applyCode.asmFile);
        }

        SaveROM(applyCode.binFile);
    } else {
        GtkWidget *message_label = GTK_WIDGET(g_object_get_data(G_OBJECT(text_view), "message-status-label"));

        if (message_label != NULL) {
            gtk_label_set_text(GTK_LABEL(message_label), "Export canceled");
        }
    }
}


void
on_project_save_as_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    (void)menuitem;
    (void)user_data;

    GtkWidget *text_view = GTK_WIDGET(user_data);
    GtkWindow *parent_window = NULL;

    if (text_view != NULL) {
        GtkWidget *toplevel = gtk_widget_get_toplevel(text_view);
        if (GTK_IS_WINDOW(toplevel)) {
            parent_window = GTK_WINDOW(toplevel);
        }
    }
    
    // save file requester asks for path 
    // then save file
    applyCode.asmFile = SaveProjectAs(parent_window);

    // save file
    if(applyCode.asmFile != "") {
        SaveProject(applyCode.asmFile);
    } else {
        GtkWidget *message_label = GTK_WIDGET(g_object_get_data(G_OBJECT(text_view), "message-status-label"));

        if (message_label != NULL) {
            gtk_label_set_text(GTK_LABEL(message_label), "Save canceled");
        }
    }
}

void
on_project_quit_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    (void)menuitem;
    GtkWindow *parent_window = GTK_WINDOW(user_data);
    gtk_window_close(parent_window);
}

void
on_edit_undo_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    (void)menuitem;

    GtkWidget *message_label = GTK_WIDGET(user_data);

    if (!GTK_IS_TEXT_VIEW(main_text_view)) {
        if (message_label != NULL) {
            gtk_label_set_text(GTK_LABEL(message_label), "Editor view unavailable");
        }
        return;
    }

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(main_text_view));
    if (buffer == NULL) {
        if (message_label != NULL) {
            gtk_label_set_text(GTK_LABEL(message_label), "No editor buffer available");
        }
        return;
    }

    if (g_undo_stack.empty()) {
        if (message_label != NULL) {
            gtk_label_set_text(GTK_LABEL(message_label), "Nothing to undo");
        }
        return;
    }

    std::string previous_text = g_undo_stack.back();
    g_undo_stack.pop_back();

    g_redo_stack.push_back(g_current_text);
    if (g_redo_stack.size() > g_max_history_entries) {
        g_redo_stack.erase(g_redo_stack.begin());
    }

    g_is_applying_history = true;
    gtk_text_buffer_set_text(buffer, previous_text.c_str(), -1);
    g_is_applying_history = false;

    g_current_text = previous_text;

    GtkTextIter start_iter;
    gtk_text_buffer_get_start_iter(buffer, &start_iter);
    gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(main_text_view), &start_iter, 0.0, FALSE, 0.0, 0.0);

    apply_editor_highlighting(buffer);

    if (message_label != NULL) {
        gtk_label_set_text(GTK_LABEL(message_label), "Undo");
    }
}

void
on_edit_redo_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    (void)menuitem;

    GtkWidget *message_label = GTK_WIDGET(user_data);

    if (!GTK_IS_TEXT_VIEW(main_text_view)) {
        if (message_label != NULL) {
            gtk_label_set_text(GTK_LABEL(message_label), "Editor view unavailable");
        }
        return;
    }

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(main_text_view));
    if (buffer == NULL) {
        if (message_label != NULL) {
            gtk_label_set_text(GTK_LABEL(message_label), "No editor buffer available");
        }
        return;
    }

    if (g_redo_stack.empty()) {
        if (message_label != NULL) {
            gtk_label_set_text(GTK_LABEL(message_label), "Nothing to redo");
        }
        return;
    }

    std::string next_text = g_redo_stack.back();
    g_redo_stack.pop_back();

    g_undo_stack.push_back(g_current_text);
    if (g_undo_stack.size() > g_max_history_entries) {
        g_undo_stack.erase(g_undo_stack.begin());
    }

    g_is_applying_history = true;
    gtk_text_buffer_set_text(buffer, next_text.c_str(), -1);
    g_is_applying_history = false;

    g_current_text = next_text;

    GtkTextIter start_iter;
    gtk_text_buffer_get_start_iter(buffer, &start_iter);
    gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(main_text_view), &start_iter, 0.0, FALSE, 0.0, 0.0);

    apply_editor_highlighting(buffer);

    if (message_label != NULL) {
        gtk_label_set_text(GTK_LABEL(message_label), "Redo");
    }
}

void
on_edit_select_all_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    (void)menuitem;

    GtkWidget *message_label = GTK_WIDGET(user_data);

    if (!GTK_IS_TEXT_VIEW(main_text_view)) {
        if (message_label != NULL) {
            gtk_label_set_text(GTK_LABEL(message_label), "Editor view unavailable");
        }
        return;
    }

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(main_text_view));
    if (buffer == NULL) {
        if (message_label != NULL) {
            gtk_label_set_text(GTK_LABEL(message_label), "No editor buffer available");
        }
        return;
    }

    GtkTextIter start_iter;
    GtkTextIter end_iter;
    gtk_text_buffer_get_bounds(buffer, &start_iter, &end_iter);
    gtk_text_buffer_select_range(buffer, &start_iter, &end_iter);

    gtk_widget_grab_focus(main_text_view);

    if (message_label != NULL) {
        gtk_label_set_text(GTK_LABEL(message_label), "All text selected");
    }
}

static gboolean
search_text_forward(GtkTextBuffer *buffer,
                    const std::string &needle,
                    GtkTextSearchFlags flags,
                    GtkWidget *message_label)
{
    if (buffer == NULL || needle.empty()) {
        if (message_label != NULL && needle.empty()) {
            gtk_label_set_text(GTK_LABEL(message_label), "Use Search (Ctrl+F) first");
        }
        return FALSE;
    }

    GtkTextIter search_start;
    gtk_text_buffer_get_iter_at_mark(buffer, &search_start, gtk_text_buffer_get_insert(buffer));

    GtkTextIter selection_start;
    GtkTextIter selection_end;
    if (gtk_text_buffer_get_selection_bounds(buffer, &selection_start, &selection_end)) {
        search_start = selection_end;
    }

    GtkTextIter match_start;
    GtkTextIter match_end;
    gboolean found = gtk_text_iter_forward_search(&search_start,
                                                  needle.c_str(),
                                                  flags,
                                                  &match_start,
                                                  &match_end,
                                                  NULL);

    if (!found) {
        gtk_text_buffer_get_start_iter(buffer, &search_start);
        found = gtk_text_iter_forward_search(&search_start,
                                             needle.c_str(),
                                             flags,
                                             &match_start,
                                             &match_end,
                                             NULL);
    }

    if (found) {
        gtk_text_buffer_select_range(buffer, &match_start, &match_end);
        gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(main_text_view), &match_start, 0.1, FALSE, 0.0, 0.0);
        if (message_label != NULL) {
            gtk_label_set_text(GTK_LABEL(message_label), "Match found");
        }
        return TRUE;
    }

    if (message_label != NULL) {
        gtk_label_set_text(GTK_LABEL(message_label), "Text not found");
    }
    return FALSE;
}

void
on_edit_search_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    (void)menuitem;

    GtkWidget *message_label = GTK_WIDGET(user_data);

    if (GTK_IS_TEXT_VIEW(main_text_view)) {
        GtkWidget *existing_dialog = GTK_WIDGET(g_object_get_data(G_OBJECT(main_text_view), "search-dialog"));
        if (GTK_IS_WINDOW(existing_dialog)) {
            gtk_window_present(GTK_WINDOW(existing_dialog));
            return;
        }
    }

    if (!GTK_IS_TEXT_VIEW(main_text_view)) {
        if (message_label != NULL) {
            gtk_label_set_text(GTK_LABEL(message_label), "Editor view unavailable");
        }
        return;
    }

    gboolean triggered_by_f3 = FALSE;
    GdkEvent *event = gtk_get_current_event();
    if (event != NULL) {
        if (event->type == GDK_KEY_PRESS) {
            const GdkEventKey *key_event = reinterpret_cast<const GdkEventKey *>(event);
            if (key_event->keyval == GDK_KEY_F3 && (key_event->state & (GDK_CONTROL_MASK | GDK_MOD1_MASK | GDK_SUPER_MASK | GDK_HYPER_MASK)) == 0) {
                triggered_by_f3 = TRUE;
            }
        }
        gdk_event_free(event);
    }

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(main_text_view));
    GtkTextIter insert_iter;

    if (triggered_by_f3) {
        if (g_last_search_text.empty()) {
            search_text_forward(buffer, g_last_search_text, g_last_search_flags, message_label);
            return;
        }

        gtk_text_buffer_get_iter_at_mark(buffer, &insert_iter, gtk_text_buffer_get_insert(buffer));

        GtkTextIter line_start_iter = insert_iter;
        gtk_text_iter_set_line_offset(&line_start_iter, 0);
        gtk_text_buffer_select_range(buffer, &line_start_iter, &line_start_iter);

        GtkTextIter line_end_iter = line_start_iter;
        gtk_text_iter_forward_to_line_end(&line_end_iter);
        gtk_text_buffer_place_cursor(buffer, &line_end_iter);

        search_text_forward(buffer, g_last_search_text, g_last_search_flags, message_label);
        return;
    }

    GtkWidget *toplevel = gtk_widget_get_toplevel(main_text_view);
    if (!GTK_IS_WINDOW(toplevel)) {
        if (message_label != NULL) {
            gtk_label_set_text(GTK_LABEL(message_label), "Unable to open search dialog");
        }
        return;
    }

    GtkWidget *dialog = gtk_dialog_new_with_buttons("Search",
                                                    GTK_WINDOW(toplevel),
                                                    static_cast<GtkDialogFlags>(GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
                                                    "_Cancel", GTK_RESPONSE_CANCEL,
                                                    "_Search", GTK_RESPONSE_OK,
                                                    NULL);

    g_object_set_data(G_OBJECT(main_text_view), "search-dialog", dialog);

    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_container_add(GTK_CONTAINER(content_area), box);

    g_signal_connect(dialog, "destroy", G_CALLBACK(on_search_dialog_destroy), main_text_view);

    gtk_text_buffer_get_iter_at_mark(buffer, &insert_iter, gtk_text_buffer_get_insert(buffer));

    GtkWidget *label = gtk_label_new("Find what:");
    gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
    gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 0);

    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_activates_default(GTK_ENTRY(entry), TRUE);
    gtk_box_pack_start(GTK_BOX(box), entry, FALSE, FALSE, 0);

    GtkWidget *match_case_check = gtk_check_button_new_with_label("Match case");
    gtk_box_pack_start(GTK_BOX(box), match_case_check, FALSE, FALSE, 0);

    gtk_widget_show_all(dialog);
    gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);

    while (true) {
        gint response = gtk_dialog_run(GTK_DIALOG(dialog));
        if (response != GTK_RESPONSE_OK) {
            break;
        }

        const gchar *search_text = gtk_entry_get_text(GTK_ENTRY(entry));
        if (search_text == NULL || *search_text == '\0') {
            if (message_label != NULL) {
                gtk_label_set_text(GTK_LABEL(message_label), "Enter text to search for");
            }
            continue;
        }

        GtkTextSearchFlags flags = GTK_TEXT_SEARCH_TEXT_ONLY;
        if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(match_case_check))) {
            flags = static_cast<GtkTextSearchFlags>(flags | GTK_TEXT_SEARCH_CASE_INSENSITIVE);
        }

        g_last_search_text = search_text;
        g_last_search_flags = flags;

        gboolean found = search_text_forward(buffer, g_last_search_text, g_last_search_flags, message_label);
        if (!found) {
            if (message_label != NULL) {
                gtk_label_set_text(GTK_LABEL(message_label), "Text not found");
            }
        }
        break;
    }

    gtk_widget_destroy(dialog);
}

void on_search_dialog_destroy(GtkWidget *widget, gpointer user_data)
{
    (void)widget;
    GtkWidget *view = GTK_WIDGET(user_data);
    if (GTK_IS_WIDGET(view)) {
        g_object_set_data(G_OBJECT(view), "search-dialog", NULL);
    }
}

void
on_edit_replace_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    (void)menuitem;

    GtkWidget *message_label = GTK_WIDGET(user_data);

    if (!GTK_IS_TEXT_VIEW(main_text_view)) {
        if (message_label != NULL) {
            gtk_label_set_text(GTK_LABEL(message_label), "Editor view unavailable");
        }
        return;
    }

    GtkWidget *toplevel = gtk_widget_get_toplevel(main_text_view);
    if (!GTK_IS_WINDOW(toplevel)) {
        if (message_label != NULL) {
            gtk_label_set_text(GTK_LABEL(message_label), "Unable to open replace dialog");
        }
        return;
    }

    GtkWidget *dialog = gtk_dialog_new_with_buttons("Replace",
                                                    GTK_WINDOW(toplevel),
                                                    static_cast<GtkDialogFlags>(GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
                                                    "_Cancel", GTK_RESPONSE_CANCEL,
                                                    "Replace _All", GTK_RESPONSE_APPLY,
                                                    "_Find Next", GTK_RESPONSE_ACCEPT,
                                                    "_Replace", GTK_RESPONSE_OK,
                                                    NULL);

    g_signal_connect(dialog, "destroy", G_CALLBACK(on_search_dialog_destroy), main_text_view);

    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 6);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 6);
    gtk_container_add(GTK_CONTAINER(content_area), grid);

    GtkWidget *find_label = gtk_label_new("Find what:");
    gtk_label_set_xalign(GTK_LABEL(find_label), 0.0f);
    gtk_grid_attach(GTK_GRID(grid), find_label, 0, 0, 1, 1);

    GtkWidget *find_entry = gtk_entry_new();
    gtk_entry_set_activates_default(GTK_ENTRY(find_entry), TRUE);
    gtk_grid_attach(GTK_GRID(grid), find_entry, 1, 0, 1, 1);

    GtkWidget *replace_label = gtk_label_new("Replace with:");
    gtk_label_set_xalign(GTK_LABEL(replace_label), 0.0f);
    gtk_grid_attach(GTK_GRID(grid), replace_label, 0, 1, 1, 1);

    GtkWidget *replace_entry = gtk_entry_new();
    gtk_entry_set_activates_default(GTK_ENTRY(replace_entry), TRUE);
    gtk_grid_attach(GTK_GRID(grid), replace_entry, 1, 1, 1, 1);

    GtkWidget *match_case_check = gtk_check_button_new_with_label("Match case");
    gtk_grid_attach(GTK_GRID(grid), match_case_check, 1, 2, 1, 1);

    gtk_widget_show_all(dialog);
    gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_ACCEPT);

    GtkWidget *find_next_button = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_ACCEPT);
    if (GTK_IS_WIDGET(find_next_button)) {
        gtk_widget_grab_focus(find_next_button);
    }

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(main_text_view));

    auto perform_search = [&](GtkTextIter &start_iter, GtkTextIter &match_start, GtkTextIter &match_end) -> gboolean {
        const gchar *needle = gtk_entry_get_text(GTK_ENTRY(find_entry));
        if (needle == NULL || *needle == '\0') {
            if (message_label != NULL) {
                gtk_label_set_text(GTK_LABEL(message_label), "Enter text to search for");
            }
            return FALSE;
        }

        GtkTextSearchFlags flags = GTK_TEXT_SEARCH_TEXT_ONLY;
        if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(match_case_check))) {
            flags = static_cast<GtkTextSearchFlags>(flags | GTK_TEXT_SEARCH_CASE_INSENSITIVE);
        }

        gboolean found = gtk_text_iter_forward_search(&start_iter,
                                                      needle,
                                                      flags,
                                                      &match_start,
                                                      &match_end,
                                                      NULL);
        if (!found) {
            gtk_text_buffer_get_start_iter(buffer, &start_iter);
            found = gtk_text_iter_forward_search(&start_iter,
                                                 needle,
                                                 flags,
                                                 &match_start,
                                                 &match_end,
                                                 NULL);
        }

        if (!found && message_label != NULL) {
            gtk_label_set_text(GTK_LABEL(message_label), "Text not found");
        }
        return found;
    };

    while (true) {
        gint response = gtk_dialog_run(GTK_DIALOG(dialog));
        if (response == GTK_RESPONSE_CANCEL || response == GTK_RESPONSE_DELETE_EVENT || response == GTK_RESPONSE_NONE) {
            break;
        }

        GtkTextIter start_iter;
        gtk_text_buffer_get_iter_at_mark(buffer, &start_iter, gtk_text_buffer_get_insert(buffer));

        GtkTextIter selection_start;
        GtkTextIter selection_end;
        if (gtk_text_buffer_get_selection_bounds(buffer, &selection_start, &selection_end)) {
            start_iter = selection_end;
        }

        GtkTextIter match_start;
        GtkTextIter match_end;
        gboolean found = perform_search(start_iter, match_start, match_end);
        if (!found) {
            continue;
        }

        if (response == GTK_RESPONSE_ACCEPT || response == GTK_RESPONSE_OK) {
            gtk_text_buffer_select_range(buffer, &match_start, &match_end);
            gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(main_text_view), &match_start, 0.1, FALSE, 0.0, 0.0);
        }

        if (response == GTK_RESPONSE_OK || response == GTK_RESPONSE_APPLY) {
            const gchar *replacement = gtk_entry_get_text(GTK_ENTRY(replace_entry));
            gtk_text_buffer_delete(buffer, &match_start, &match_end);
            if (replacement != NULL) {
                gtk_text_buffer_insert(buffer, &match_start, replacement, -1);
            }

            if (response == GTK_RESPONSE_APPLY) {
                GtkTextIter next_start = match_start;
                if (replacement != NULL && *replacement != '\0') {
                    gtk_text_iter_forward_chars(&next_start, g_utf8_strlen(replacement, -1));
                }

                while (perform_search(next_start, match_start, match_end)) {
                    gtk_text_buffer_delete(buffer, &match_start, &match_end);
                    if (replacement != NULL) {
                        gtk_text_buffer_insert(buffer, &match_start, replacement, -1);
                    }
                    next_start = match_start;
                    if (replacement != NULL && *replacement != '\0') {
                        gtk_text_iter_forward_chars(&next_start, g_utf8_strlen(replacement, -1));
                    }
                }

                if (message_label != NULL) {
                    gtk_label_set_text(GTK_LABEL(message_label), "Replace All completed");
                }
            } else {
                gtk_text_buffer_get_iter_at_mark(buffer, &match_start, gtk_text_buffer_get_insert(buffer));
                if (message_label != NULL) {
                    gtk_label_set_text(GTK_LABEL(message_label), "Replacement done");
                }
            }
        }
    }

    gtk_widget_destroy(dialog);
}

void
on_edit_goto_line_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    (void)menuitem;

    GtkWidget *message_label = GTK_WIDGET(user_data);

    if (!GTK_IS_TEXT_VIEW(main_text_view)) {
        if (message_label != NULL) {
            gtk_label_set_text(GTK_LABEL(message_label), "Editor view unavailable");
        }
        return;
    }

    GtkWidget *toplevel = gtk_widget_get_toplevel(main_text_view);
    if (!GTK_IS_WINDOW(toplevel)) {
        if (message_label != NULL) {
            gtk_label_set_text(GTK_LABEL(message_label), "Unable to open goto dialog");
        }
        return;
    }

    GtkWidget *dialog = gtk_dialog_new_with_buttons("Goto Line",
                                                    GTK_WINDOW(toplevel),
                                                    static_cast<GtkDialogFlags>(GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
                                                    "_Cancel", GTK_RESPONSE_CANCEL,
                                                    "_Go To", GTK_RESPONSE_OK,
                                                    NULL);

    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_container_add(GTK_CONTAINER(content_area), box);

    GtkWidget *label = gtk_label_new("Line number:");
    gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
    gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 0);

    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_activates_default(GTK_ENTRY(entry), TRUE);
    gtk_box_pack_start(GTK_BOX(box), entry, FALSE, FALSE, 0);

    GtkWidget *error_label = gtk_label_new("");
    gtk_label_set_xalign(GTK_LABEL(error_label), 0.0f);
    gtk_box_pack_start(GTK_BOX(box), error_label, FALSE, FALSE, 0);

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(main_text_view));
    if (buffer == NULL) {
        if (message_label != NULL) {
            gtk_label_set_text(GTK_LABEL(message_label), "No editor buffer available");
        }
        gtk_widget_destroy(dialog);
        return;
    }

    gint current_line_number = 1;
    GtkTextIter cursor_iter;
    gtk_text_buffer_get_iter_at_mark(buffer, &cursor_iter, gtk_text_buffer_get_insert(buffer));
    current_line_number = gtk_text_iter_get_line(&cursor_iter) + 1;

    gchar current_line_text[32];
    g_snprintf(current_line_text, sizeof(current_line_text), "%d", current_line_number);
    gtk_entry_set_text(GTK_ENTRY(entry), current_line_text);

    gtk_widget_show_all(dialog);
    gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);

    bool success = false;

    while (!success) {
        gint response = gtk_dialog_run(GTK_DIALOG(dialog));
        if (response != GTK_RESPONSE_OK) {
            gtk_widget_destroy(dialog);
            return;
        }

        const gchar *line_text = gtk_entry_get_text(GTK_ENTRY(entry));
        if (line_text == NULL || *line_text == '\0') {
            gtk_label_set_text(GTK_LABEL(error_label), "Enter a line number");
            continue;
        }

        errno = 0;
        char *endptr = NULL;
        long parsed = std::strtol(line_text, &endptr, 10);
        if (errno != 0 || endptr == line_text || (endptr != NULL && *endptr != '\0')) {
            gtk_label_set_text(GTK_LABEL(error_label), "Invalid number format");
            continue;
        }

        if (parsed <= 0 || parsed > G_MAXINT) {
            gtk_label_set_text(GTK_LABEL(error_label), "Enter a positive number");
            continue;
        }

        gint target_line = static_cast<gint>(parsed);
        gint line_count = gtk_text_buffer_get_line_count(buffer);
        if (line_count <= 0) {
            if (message_label != NULL) {
                gtk_label_set_text(GTK_LABEL(message_label), "Document is empty");
            }
            gtk_widget_destroy(dialog);
            return;
        }

        if (target_line > line_count) {
            std::string error_text = "Line number must be between 1 and " + std::to_string(line_count);
            gtk_label_set_text(GTK_LABEL(error_label), error_text.c_str());
            continue;
        }

        GtkTextIter target_iter;
        gtk_text_buffer_get_iter_at_line(buffer, &target_iter, target_line - 1);
        gtk_text_buffer_place_cursor(buffer, &target_iter);
        gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(main_text_view), &target_iter, 0.1, FALSE, 0.0, 0.0);
        gtk_widget_grab_focus(main_text_view);

        if (message_label != NULL) {
            std::string success_text = "Moved to line " + std::to_string(target_line);
            gtk_label_set_text(GTK_LABEL(message_label), success_text.c_str());
        }

        success = true;
    }

    gtk_widget_destroy(dialog);
}

static GtkAccelGroup *
ensure_accel_group_for_widget(GtkWidget *widget)
{
    if (widget == NULL) {
        return NULL;
    }

    GtkWidget *toplevel = gtk_widget_get_toplevel(widget);
    if (!GTK_IS_WINDOW(toplevel)) {
        return NULL;
    }

    GSList *existing_groups = gtk_accel_groups_from_object(G_OBJECT(toplevel));
    if (existing_groups != NULL) {
        GtkAccelGroup *group = static_cast<GtkAccelGroup *>(existing_groups->data);
        return group;
    }

    GtkAccelGroup *new_group = gtk_accel_group_new();
    gtk_window_add_accel_group(GTK_WINDOW(toplevel), new_group);
    return new_group;
}

static void
clear_menu_item_accelerator(GtkWidget *item)
{
    if (item == NULL) return;

    // Récupère la liste des closures d'accélérateurs liées à item
    GList *list = gtk_widget_list_accel_closures(item);

    for (GList *l = list; l != NULL; l = l->next) {
        GClosure *closure = (GClosure *) l->data;

        /* trouve le GtkAccelGroup lié à cette closure */
        GtkAccelGroup *ag = gtk_accel_group_from_accel_closure(closure);
        if (ag != NULL) {
            /* déconnecte la closure (supprime l'accélérateur) */
            gtk_accel_group_disconnect(ag, closure);
        }
    }
    /* libère la structure de liste ; ne pas g_closure_unref() ici :
       les closures sont gérées par le widget. */
    g_list_free(list);

    /* Optionnel : si c'est un GtkMenuItem, dissocier son accel-path
       (utile si vous utilisez l'accel-map / accel-paths). */
    if (GTK_IS_MENU_ITEM(item)) {
        gtk_menu_item_set_accel_path(GTK_MENU_ITEM(item), NULL);
    }
}
static void
set_menu_accelerator(GtkWidget *item,
                     GtkWidget *toplevel,
                     guint keyval,
                     GdkModifierType modifiers)
{
    GtkAccelGroup *accel_group = ensure_accel_group_for_widget(toplevel);

    if (accel_group == NULL) {
        return;
    }

    clear_menu_item_accelerator(item);

    gtk_widget_add_accelerator(item,
                               "activate",
                               accel_group,
                               keyval,
                               modifiers,
                               GTK_ACCEL_VISIBLE);
}

void
on_compiler_build_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    (void)menuitem;

    GtkWidget *message_label = GTK_WIDGET(user_data);

    if (message_label != NULL) {
        gtk_label_set_text(GTK_LABEL(message_label), "Building source code");
    }

    Build();
}

void
on_compiler_build_and_run_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    (void)menuitem;
    GtkWidget *message_label = GTK_WIDGET(user_data);

    if (message_label != NULL) {
        gtk_label_set_text(GTK_LABEL(message_label), "Build and run");
        gtk_widget_queue_draw(message_label);
    }

    // Perform the build step first
    if(Build()) {
        if (applyCode.emuFile.empty()) {
            if (message_label != NULL) {
                gtk_label_set_text(GTK_LABEL(message_label), "No Monster8 emulator found");
            }
            return;
        }

        if (applyCode.binFile.empty()) {
            if (message_label != NULL) {
                gtk_label_set_text(GTK_LABEL(message_label), "No binm8 file found");
            }
            return;
        }

        // Export the current project and ROM before launching the emulator
        SaveProject(applyCode.asmFile);
        SaveROM(applyCode.binFile);

        if (message_label != NULL) {
            gtk_label_set_text(GTK_LABEL(message_label), "Program compiled and saved. Launching emulator...");
            gtk_widget_queue_draw(message_label);
        }

        gchar *argv[] = {
            const_cast<gchar *>(applyCode.emuFile.c_str()),
            const_cast<gchar *>("-w"),
            const_cast<gchar *>(applyCode.binFile.c_str()),
            NULL
        };

        GError *spawn_error = NULL;
        if (!g_spawn_async(NULL,
                        argv,
                        NULL,
                        G_SPAWN_SEARCH_PATH,
                        NULL,
                        NULL,
                        NULL,
                        &spawn_error)) {
            if (message_label != NULL) {
                gtk_label_set_text(GTK_LABEL(message_label), "Failed to launch emulator");
            }

            if (spawn_error != NULL) {
                g_warning("Failed to launch emulator '%s': %s", applyCode.emuFile.c_str(), spawn_error->message);
                g_error_free(spawn_error);
            }
            return;
        }

        if (message_label != NULL) {
            gtk_label_set_text(GTK_LABEL(message_label), "Emulator launched");
        }
    }
}

void
on_medias_import_spritesheet_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    (void)menuitem;
    GtkWidget *text_view = GTK_WIDGET(user_data);
    GtkWindow *parent_window = NULL;

    if (text_view != NULL) {
        GtkWidget *toplevel = gtk_widget_get_toplevel(text_view);
        if (GTK_IS_WINDOW(toplevel)) {
            parent_window = GTK_WINDOW(toplevel);
        }
    }

    GtkWidget *message_label = GTK_WIDGET(user_data);

    gtk_label_set_text(GTK_LABEL(message_label), "Import spritesheet");

    // get address to store spritesheet data, from inputbox
    GtkWidget *address_dialog = gtk_dialog_new_with_buttons(
        "Sprite Sheet Address",
        parent_window,
        GTK_DIALOG_MODAL,
        "_Cancel",
        GTK_RESPONSE_CANCEL,
        "_OK",
        GTK_RESPONSE_OK,
        NULL);

    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(address_dialog));

    GtkWidget *prompt_label = gtk_label_new("Enter sprite sheet start address (hex):");
    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(entry), 8); // limit to 32-bit hex

    gtk_entry_set_text(GTK_ENTRY(entry), Hex(applyCode.nextSpriteAddr).c_str());

    gtk_box_pack_start(GTK_BOX(content_area), prompt_label, FALSE, FALSE, 4);
    gtk_box_pack_start(GTK_BOX(content_area), entry, FALSE, FALSE, 4);

    gtk_widget_show_all(address_dialog);

    uint32_t addr = 0;

    gint response = gtk_dialog_run(GTK_DIALOG(address_dialog));

    if (response == GTK_RESPONSE_OK) {
        const gchar *addr_text = gtk_entry_get_text(GTK_ENTRY(entry));
        if (addr_text != NULL && *addr_text != '\0') {
            addr = Val(addr_text);
        }
    } else {
        gtk_label_set_text(GTK_LABEL(message_label), "Import canceled");
        gtk_widget_destroy(address_dialog);

        return;
    }

    gtk_widget_destroy(address_dialog);

    uint32_t addr0 = addr;

    // Open the spritesheet file chooser
    GtkWidget *spritesheet_dialog = gtk_file_chooser_dialog_new("Import spritesheet",
                                                                parent_window,
                                                                GTK_FILE_CHOOSER_ACTION_OPEN,
                                                                "_Cancel", GTK_RESPONSE_CANCEL,
                                                                "_Open", GTK_RESPONSE_ACCEPT,
                                                                NULL);

    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "png files (*.png)");
    gtk_file_filter_add_pattern(filter, "*.png");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(spritesheet_dialog), filter);

    gint file_response = gtk_dialog_run(GTK_DIALOG(spritesheet_dialog));
    if (file_response == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(spritesheet_dialog));

        if (filename != NULL) {
            GError *pixbuf_error = NULL;
            GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(filename, &pixbuf_error);

            if (pixbuf == NULL) {
                const gchar *error_text = pixbuf_error != NULL && pixbuf_error->message != NULL
                                                  ? pixbuf_error->message
                                                  : "Failed to load PNG";
                    gtk_label_set_text(GTK_LABEL(message_label), error_text);

                if (pixbuf_error != NULL) {
                    g_error_free(pixbuf_error);
                }

                g_free(filename);
                gtk_widget_destroy(spritesheet_dialog);
                return;
            }

            gint image_width = gdk_pixbuf_get_width(pixbuf);
            gint image_height = gdk_pixbuf_get_height(pixbuf);

            gint frame_width = 16;
            gint frame_height = 16;

            gtk_widget_destroy(spritesheet_dialog);

            if (!prompt_for_frame_dimensions(parent_window, image_width, image_height, frame_width, frame_height)) {
                g_object_unref(pixbuf);
                g_free(filename);
                
                return;
            }

            if(frame_width > 256 || frame_height > 256 || frame_width < 1 || frame_height < 1) {
                gtk_label_set_text(GTK_LABEL(message_label), "Wrong frame dimensions");
            } else if (image_width % frame_width != 0 || image_height % frame_height != 0) {
                gtk_label_set_text(GTK_LABEL(message_label), "Wrong frame or sprite sheet size");
            } else {
                uint32_t y = 0;

                while (y < static_cast<uint32_t>(image_height)) {
                    uint32_t x = 0;

                    while (x < static_cast<uint32_t>(image_width)) {
                        // fill sprite infos
                        applyCode.spriteCount++;

                        applyCode.spritePath.push_back(filename);
                        applyCode.spriteAddress.push_back(addr);
                        applyCode.spriteLength.push_back(frame_width * frame_height);
                        applyCode.spriteWidth.push_back(frame_width);
                        applyCode.spriteHeight.push_back(frame_height);

                        addr = CopyImageToRaw(pixbuf, addr + 0x1000000, x, y, frame_width, frame_height) - 0x1000000;
                        
                        if (addr >= 0x1FFFFFF) {
                            gtk_label_set_text(GTK_LABEL(message_label), "Memory full");
                        }

                        x += static_cast<uint32_t>(frame_width);
                    }

                    y += static_cast<uint32_t>(frame_height);
                }

                // new addr
                applyCode.nextSpriteAddr = addr0 + addr;
            }

            g_object_unref(pixbuf);
            g_free(filename);
        } else {
            gtk_label_set_text(GTK_LABEL(message_label), "Import canceled");
        }
    }
}

void
on_medias_import_tileset_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    (void)menuitem;
    GtkWidget *message_label = GTK_WIDGET(user_data);
    GtkWindow *parent_window = NULL;

    if (message_label != NULL) {
        gtk_label_set_text(GTK_LABEL(message_label), "Import tileset");
    }

    // get address to store spritesheet data, from inputbox
    GtkWidget *address_dialog = gtk_dialog_new_with_buttons(
        "Tileset Address",
        parent_window,
        GTK_DIALOG_MODAL,
        "_Cancel",
        GTK_RESPONSE_CANCEL,
        "_OK",
        GTK_RESPONSE_OK,
        NULL);

    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(address_dialog));

    GtkWidget *prompt_label = gtk_label_new("Enter tileset start address (hex):");
    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(entry), 8); // limit to 32-bit hex

    gtk_entry_set_text(GTK_ENTRY(entry), Hex(applyCode.nextTilesetAddr).c_str());

    gtk_box_pack_start(GTK_BOX(content_area), prompt_label, FALSE, FALSE, 4);
    gtk_box_pack_start(GTK_BOX(content_area), entry, FALSE, FALSE, 4);

    gtk_widget_show_all(address_dialog);

    uint32_t addr = 0;

    gint response = gtk_dialog_run(GTK_DIALOG(address_dialog));

    if (response == GTK_RESPONSE_OK) {
        const gchar *addr_text = gtk_entry_get_text(GTK_ENTRY(entry));
        if (addr_text != NULL && *addr_text != '\0') {
            addr = Val(addr_text);
        }
    } else {
        gtk_label_set_text(GTK_LABEL(message_label), "Import canceled");
        gtk_widget_destroy(address_dialog);

        return;
    }

    gtk_widget_destroy(address_dialog);

    uint32_t addr0 = addr;

    // Open the spritesheet file chooser
    GtkWidget *spritesheet_dialog = gtk_file_chooser_dialog_new("Import tileset",
                                                                parent_window,
                                                                GTK_FILE_CHOOSER_ACTION_OPEN,
                                                                "_Cancel", GTK_RESPONSE_CANCEL,
                                                                "_Open", GTK_RESPONSE_ACCEPT,
                                                                NULL);

    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "png files (*.png)");
    gtk_file_filter_add_pattern(filter, "*.png");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(spritesheet_dialog), filter);

    gint file_response = gtk_dialog_run(GTK_DIALOG(spritesheet_dialog));
    if (file_response == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(spritesheet_dialog));

        if (filename != NULL) {
            GError *pixbuf_error = NULL;
            GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(filename, &pixbuf_error);

            if (pixbuf == NULL) {
                const gchar *error_text = pixbuf_error != NULL && pixbuf_error->message != NULL
                                                  ? pixbuf_error->message
                                                  : "Failed to load PNG";
                    gtk_label_set_text(GTK_LABEL(message_label), error_text);

                if (pixbuf_error != NULL) {
                    g_error_free(pixbuf_error);
                }

                g_free(filename);
                gtk_widget_destroy(spritesheet_dialog);
                return;
            }

            gint image_width = gdk_pixbuf_get_width(pixbuf);
            gint image_height = gdk_pixbuf_get_height(pixbuf);

            gint tile_width = 16;
            gint tile_height = 16;

            gtk_widget_destroy(spritesheet_dialog);

            if (!prompt_for_frame_dimensions(parent_window, image_width, image_height, tile_width, tile_height)) {
                g_object_unref(pixbuf);
                g_free(filename);
                
                return;
            }

            if(tile_width > 128 || tile_height > 128 || tile_width < 8 || tile_height < 8) {
                gtk_label_set_text(GTK_LABEL(message_label), "Wrong tile dimensions");
            } else if(image_width % tile_width != 0 || image_height % tile_height != 0) {
                gtk_label_set_text(GTK_LABEL(message_label), "Wrong tile or tileset size");
            } else {
                uint32_t y = 0;

                while (y < static_cast<uint32_t>(image_height)) {
                    uint32_t x = 0;

                    while (x < static_cast<uint32_t>(image_width)) {
                        // fill sprite infos
                        applyCode.tilesetCount++;

                        applyCode.tilesetPath.push_back(filename);
                        applyCode.tilesetAddress.push_back(addr);
                        applyCode.tilesetLength.push_back(tile_width * tile_height);
                        applyCode.tilesetWidth.push_back(tile_width);
                        applyCode.tilesetHeight.push_back(tile_height);

                        addr = CopyImageToRaw(pixbuf, addr + 0x2000000, x, y, tile_width, tile_height) - 0x2000000;
                        
                        if (addr >= 0x2FFFFFF) {
                            gtk_label_set_text(GTK_LABEL(message_label), "Memory full");
                        }

                        x += static_cast<uint32_t>(tile_width);
                    }

                    y += static_cast<uint32_t>(tile_height);
                }

                // new addr
                applyCode.nextTilesetAddr = addr0 + addr;
            }

            g_object_unref(pixbuf);
            g_free(filename);
        } else {
            gtk_label_set_text(GTK_LABEL(message_label), "Import canceled");
        }
    }
}

void
on_medias_import_sound_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    (void)menuitem;
    GtkWidget *message_label = GTK_WIDGET(user_data);

    if (message_label != NULL) {
        gtk_label_set_text(GTK_LABEL(message_label), "Import sound");
    }

    g_print("Medias -> Import sound triggered\n");
}

void
on_medias_import_music_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    (void)menuitem;
    GtkWidget *message_label = GTK_WIDGET(user_data);

    if (message_label != NULL) {
        gtk_label_set_text(GTK_LABEL(message_label), "Import music");
    }

    g_print("Medias -> Import music triggered\n");
}

void
on_medias_clear_sprites_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    (void)menuitem;
    GtkWidget *message_label = GTK_WIDGET(user_data);

    if (message_label != NULL) {
        gtk_label_set_text(GTK_LABEL(message_label), "Clear sprites");
    }

    for(uint32_t i = 0x1000000; i <= 0x1FFFFFF; i++) {
        applyCode.memory[i] = 0x00;
    }

    applyCode.nextSpriteAddr = 0x000000;

    applyCode.spriteCount = 0;
    applyCode.spritePath.clear();
    applyCode.spriteAddress.clear();
    applyCode.spriteLength.clear();
    applyCode.spriteWidth.clear();
    applyCode.spriteHeight.clear();

    gtk_label_set_text(GTK_LABEL(message_label), "Sprites cleared");
}

void
on_medias_clear_tilesets_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    (void)menuitem;
    GtkWidget *message_label = GTK_WIDGET(user_data);

    if (message_label != NULL) {
        gtk_label_set_text(GTK_LABEL(message_label), "Clear tilesets");
    }

    for(uint32_t i = 0x2000000; i <= 0x2FFFFFF; i++) {
        applyCode.memory[i] = 0x00;
    }

    applyCode.nextTilesetAddr = 0x000000;
        
    applyCode.tilesetCount = 0;
    applyCode.tilesetPath.clear();
    applyCode.tilesetAddress.clear();
    applyCode.tilesetLength.clear();
    applyCode.tilesetWidth.clear();
    applyCode.tilesetHeight.clear();

    gtk_label_set_text(GTK_LABEL(message_label), "Tilesets cleared");
}

void
on_medias_clear_sounds_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    (void)menuitem;
    GtkWidget *message_label = GTK_WIDGET(user_data);

    if (message_label != NULL) {
        gtk_label_set_text(GTK_LABEL(message_label), "Clear sounds");
    }

    for(uint32_t i = 0x3000000; i <= 0x3FFFFFF; i++) {
        applyCode.memory[i] = 0x00;
    }

    applyCode.nextSoundAddr = 0x000000;
            
    applyCode.soundCount = 0;
    applyCode.soundPath.clear();
    applyCode.soundAddress.clear();
    applyCode.soundLength.clear();

    gtk_label_set_text(GTK_LABEL(message_label), "Sounds cleared");
}

void
on_medias_clear_musics_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    (void)menuitem;
    GtkWidget *message_label = GTK_WIDGET(user_data);

    if (message_label != NULL) {
        gtk_label_set_text(GTK_LABEL(message_label), "Clear musics");
    }

    for(uint32_t i = 0x4000000; i <= 0x4FFFFFF; i++) {
        applyCode.memory[i] = 0x00;
    }

    applyCode.nextMusicAddr = 0x000000;
            
    applyCode.musicCount = 0;
    applyCode.musicPath.clear();
    applyCode.musicAddress.clear();
    applyCode.musicLength.clear();

    gtk_label_set_text(GTK_LABEL(message_label), "Musics cleared");
}

void
on_settings_setup_emulator_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    (void)menuitem;
    GtkWidget *message_label = GTK_WIDGET(user_data);

    if (message_label != NULL) {
        gtk_label_set_text(GTK_LABEL(message_label), "Select Monster8 emulator");
    }

    GtkWidget *text_view = main_text_view;
    GtkWindow *parent_window = NULL;

    if (GTK_IS_WIDGET(text_view)) {
        GtkWidget *toplevel = gtk_widget_get_toplevel(text_view);
        if (GTK_IS_WINDOW(toplevel)) {
            parent_window = GTK_WINDOW(toplevel);
        }
    }

    GtkWidget *dialog = gtk_file_chooser_dialog_new("Select Monster8 emulator",
                                                    parent_window,
                                                    GTK_FILE_CHOOSER_ACTION_OPEN,
                                                    "_Cancel", GTK_RESPONSE_CANCEL,
                                                    "_Select", GTK_RESPONSE_ACCEPT,
                                                    NULL);

    GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
    gtk_file_chooser_set_local_only(chooser, TRUE);

#ifdef _WIN32
    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "Monster8 executable (Monster8.exe)");
    gtk_file_filter_add_pattern(filter, "Monster8.exe");
    gtk_file_chooser_add_filter(chooser, filter);
    gtk_file_chooser_set_current_name(chooser, "Monster8.exe");
#else
    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "Monster8 executable (Monster8)");
    gtk_file_filter_add_pattern(filter, "Monster8");
    gtk_file_chooser_add_filter(chooser, filter);
    gtk_file_chooser_set_current_name(chooser, "Monster8");
#endif

    if (!applyCode.emuFile.empty()) {
        gtk_file_chooser_set_filename(chooser, applyCode.emuFile.c_str());
    }

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(chooser);
        if (filename != NULL) {
            applyCode.emuFile = filename;
            g_free(filename);

            const string settings_path = "Settings.cfg";

            ofstream settings_out(settings_path, ios::out | ios::trunc);
            if (settings_out) {
                settings_out << "EmulatorPath=" + applyCode.emuFile;
            }

            if (message_label != NULL) {
                gtk_label_set_text(GTK_LABEL(message_label), "Emulator configured");
            }
        }
    }

    gtk_widget_destroy(dialog);
}

void
on_informations_about_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    (void)menuitem;
    GtkWidget *text_view = GTK_WIDGET(user_data);
    GtkWindow *parent_window = NULL;

    if (text_view != NULL) {
        GtkWidget *toplevel = gtk_widget_get_toplevel(text_view);
        if (GTK_IS_WINDOW(toplevel)) {
            parent_window = GTK_WINDOW(toplevel);
        }
    }

    GtkWidget *dialog = gtk_dialog_new_with_buttons("About Monster8 Editor",
                                                    parent_window,
                                                    GTK_DIALOG_MODAL,
                                                    "_OK",
                                                    GTK_RESPONSE_OK,
                                                    NULL);

    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(container), 20);

    GtkWidget *title_label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title_label), "<span size='large'><b>Monster8 Editor</b></span>");
    gtk_box_pack_start(GTK_BOX(container), title_label, FALSE, FALSE, 0);

    GtkWidget *version_label = gtk_label_new("Version 0.1.0");
    gtk_box_pack_start(GTK_BOX(container), version_label, FALSE, FALSE, 0);

    GtkWidget *description_label = gtk_label_new("A simple assembly editor for Monster8 fantasy console");
    gtk_box_pack_start(GTK_BOX(container), description_label, FALSE, FALSE, 0);

    GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(container), separator, FALSE, FALSE, 5);

    GtkWidget *features_label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(features_label), 
        "<b>Features:</b>\n"
        "• Assembly code editing with syntax highlighting\n"
        "• Sprite and tileset import\n"
        "• Sound and music import\n"
        "• Project management\n"
        "• Build and run functionality");
    gtk_label_set_justify(GTK_LABEL(features_label), GTK_JUSTIFY_LEFT);
    gtk_box_pack_start(GTK_BOX(container), features_label, FALSE, FALSE, 0);

    gtk_container_add(GTK_CONTAINER(content_area), container);
    gtk_widget_show_all(dialog);

    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void
on_medias_informations_activate(GtkMenuItem *menuitem, gpointer user_data)
{
    (void)menuitem;
    GtkWidget *text_view = GTK_WIDGET(user_data);
    GtkWindow *parent_window = NULL;

    if (text_view != NULL) {
        GtkWidget *toplevel = gtk_widget_get_toplevel(text_view);
        if (GTK_IS_WINDOW(toplevel)) {
            parent_window = GTK_WINDOW(toplevel);
        }
    }

    GtkWidget *dialog = gtk_dialog_new_with_buttons("Medias Informations",
                                                    parent_window,
                                                    GTK_DIALOG_MODAL,
                                                    "_OK",
                                                    GTK_RESPONSE_OK,
                                                    NULL);

    // Élargir la fenêtre de 80 pixels
    gtk_window_set_default_size(GTK_WINDOW(dialog), 400, -1);

    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(container), 20);

    GtkWidget *title_label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title_label), "<span size='large'><b>Medias Status</b></span>");
    gtk_box_pack_start(GTK_BOX(container), title_label, FALSE, FALSE, 0);

    // Informations sur les sprites
    std::string sprites_info = "Sprites: " + std::to_string(applyCode.spriteCount) + " loaded";
    GtkWidget *sprites_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *sprites_label = gtk_label_new(sprites_info.c_str());
    GtkWidget *sprites_more_btn = gtk_button_new_with_label("More...");
    g_signal_connect(sprites_more_btn, "clicked", G_CALLBACK(on_sprites_more_clicked), parent_window);
    gtk_widget_set_halign(sprites_label, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(sprites_box), sprites_label, TRUE, TRUE, 0);
    gtk_box_pack_end(GTK_BOX(sprites_box), sprites_more_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(container), sprites_box, FALSE, FALSE, 0);

    // Informations sur les tilesets
    std::string tilesets_info = "Tilesets: " + std::to_string(applyCode.tilesetCount) + " loaded";
    GtkWidget *tilesets_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *tilesets_label = gtk_label_new(tilesets_info.c_str());
    GtkWidget *tilesets_more_btn = gtk_button_new_with_label("More...");
    printf("DEBUG: Connecting tilesets More button, tilesetCount=%d\n", applyCode.tilesetCount);
    fflush(stdout);
    g_signal_connect(tilesets_more_btn, "clicked", G_CALLBACK(on_tilesets_more_clicked), parent_window);
    gtk_widget_set_halign(tilesets_label, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(tilesets_box), tilesets_label, TRUE, TRUE, 0);
    gtk_box_pack_end(GTK_BOX(tilesets_box), tilesets_more_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(container), tilesets_box, FALSE, FALSE, 0);

    // Informations sur les sons
    std::string sounds_info = "Sounds: " + std::to_string(applyCode.soundCount) + " loaded";
    GtkWidget *sounds_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *sounds_label = gtk_label_new(sounds_info.c_str());
    GtkWidget *sounds_more_btn = gtk_button_new_with_label("More...");
    gtk_widget_set_halign(sounds_label, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(sounds_box), sounds_label, TRUE, TRUE, 0);
    gtk_box_pack_end(GTK_BOX(sounds_box), sounds_more_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(container), sounds_box, FALSE, FALSE, 0);

    // Informations sur les musiques
    std::string musics_info = "Musics: " + std::to_string(applyCode.musicCount) + " loaded";
    GtkWidget *musics_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *musics_label = gtk_label_new(musics_info.c_str());
    GtkWidget *musics_more_btn = gtk_button_new_with_label("More...");
    gtk_widget_set_halign(musics_label, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(musics_box), musics_label, TRUE, TRUE, 0);
    gtk_box_pack_end(GTK_BOX(musics_box), musics_more_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(container), musics_box, FALSE, FALSE, 0);

    GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(container), separator, FALSE, FALSE, 5);

    // Informations sur les adresses mémoire
    std::string memory_info = "Memory addresses:\n"
                             "Next sprite: " + Hex(applyCode.nextSpriteAddr) + "\n"
                             "Next tileset: " + Hex(applyCode.nextTilesetAddr) + "\n"
                             "Next sound: " + Hex(applyCode.nextSoundAddr) + "\n"
                             "Next music: " + Hex(applyCode.nextMusicAddr);
    GtkWidget *memory_label = gtk_label_new(memory_info.c_str());
    gtk_label_set_justify(GTK_LABEL(memory_label), GTK_JUSTIFY_LEFT);
    gtk_box_pack_start(GTK_BOX(container), memory_label, FALSE, FALSE, 0);

    gtk_container_add(GTK_CONTAINER(content_area), container);
    gtk_widget_show_all(dialog);

    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

// Structure pour les données du callback sprite viewer (already defined in gtk.hpp)

// Fonction pour mettre à jour l'image sprite
void update_sprite_image(GtkWidget *combo, GtkWidget *image_widget) {
    uint32_t spriteNumber = static_cast<uint32_t>(gtk_combo_box_get_active(GTK_COMBO_BOX(combo)));
    
    if (spriteNumber >= applyCode.spriteCount) {
        return;
    }
    
    // Créer le pixbuf du sprite sélectionné
    GdkPixbuf *sprite_pixbuf = ConvertImageTo24bits(applyCode.spriteAddress[spriteNumber] + 0x1000000, 
                                                    applyCode.spriteWidth[spriteNumber], 
                                                    applyCode.spriteHeight[spriteNumber]);
    
    if (sprite_pixbuf != NULL) {
        // Redimensionner l'image pour s'adapter au widget 256x256 en maintenant les proportions
        gint original_width = gdk_pixbuf_get_width(sprite_pixbuf);
        gint original_height = gdk_pixbuf_get_height(sprite_pixbuf);
        
        // Calculer le facteur de zoom pour s'adapter à 256x256
        gdouble scale_x = 256.0 / original_width;
        gdouble scale_y = 256.0 / original_height;
        gdouble scale = MIN(scale_x, scale_y); // Maintenir les proportions
        
        gint new_width = (gint)(original_width * scale);
        gint new_height = (gint)(original_height * scale);
        
        // Redimensionner le pixbuf
        GdkPixbuf *scaled_pixbuf = gdk_pixbuf_scale_simple(sprite_pixbuf, new_width, new_height, GDK_INTERP_NEAREST);
        
        if (scaled_pixbuf != NULL) {
            gtk_image_set_from_pixbuf(GTK_IMAGE(image_widget), scaled_pixbuf);
            g_object_unref(scaled_pixbuf);
        } else {
            gtk_image_set_from_pixbuf(GTK_IMAGE(image_widget), sprite_pixbuf);
        }
        
        g_object_unref(sprite_pixbuf); // Libérer la référence du pixbuf original
    }
}

// Callback pour changement combobox sprite
void on_sprite_combo_changed(GtkComboBox *combo, gpointer user_data) {
    SpriteViewerData *data = static_cast<SpriteViewerData*>(user_data);
    update_sprite_image(GTK_WIDGET(combo), data->image_widget);
}

void
on_sprites_more_clicked(GtkButton *button, gpointer user_data)
{
    (void)button;
    GtkWindow *parent_window = GTK_WINDOW(user_data);
    open_sprite_viewer(parent_window);
}

void
open_sprite_viewer(GtkWindow *parent_window)
{
    GtkWidget *dialog = gtk_dialog_new_with_buttons("View Sprite",
                                                    parent_window,
                                                    GTK_DIALOG_MODAL,
                                                    "_Close",
                                                    GTK_RESPONSE_CLOSE,
                                                    NULL);

    gtk_window_set_default_size(GTK_WINDOW(dialog), 300, 400);

    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(container), 20);

    // Label et combobox pour sélectionner le sprite
    GtkWidget *sprite_label = gtk_label_new("Select Sprite:");
    gtk_widget_set_halign(sprite_label, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(container), sprite_label, FALSE, FALSE, 0);

    GtkWidget *sprite_combo = gtk_combo_box_text_new();
    for (int i = 1; i <= applyCode.spriteCount; i++) {
        std::string sprite_number = std::to_string(i);
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(sprite_combo), sprite_number.c_str());
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(sprite_combo), 0); // Sélectionner le premier par défaut
    gtk_box_pack_start(GTK_BOX(container), sprite_combo, FALSE, FALSE, 0);

    // Zone pour afficher l'image (256x256 maximum)
    GtkWidget *image_frame = gtk_frame_new("Sprite Image");
    GtkWidget *image_scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(image_scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(image_scrolled, 256, 256);
    
    GtkWidget *image_widget = gtk_image_new();
    gtk_container_add(GTK_CONTAINER(image_scrolled), image_widget);
    gtk_container_add(GTK_CONTAINER(image_frame), image_scrolled);
    gtk_box_pack_start(GTK_BOX(container), image_frame, TRUE, TRUE, 0);

    // Créer la structure pour le callback
    SpriteViewerData *viewer_data = g_new(SpriteViewerData, 1);
    viewer_data->combo = sprite_combo;
    viewer_data->image_widget = image_widget;

    // Connecter le signal changed de la combobox
    g_signal_connect(G_OBJECT(sprite_combo), "changed", G_CALLBACK(on_sprite_combo_changed), viewer_data);
    
    // Connecter la destruction du dialog pour libérer la mémoire
    g_signal_connect_swapped(G_OBJECT(dialog), "destroy", G_CALLBACK(g_free), viewer_data);

    // Charger l'image initiale
    update_sprite_image(sprite_combo, image_widget);

    gtk_container_add(GTK_CONTAINER(content_area), container);
    gtk_widget_show_all(dialog);

    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

// Tile viewer functions - similar to sprite viewer but for tilesets

// Fonction pour mettre à jour l'image tile
void update_tile_image(GtkWidget *combo, GtkWidget *image_widget) {
    printf("DEBUG: update_tile_image called\n");
    fflush(stdout);
    
    uint32_t tileNumber = static_cast<uint32_t>(gtk_combo_box_get_active(GTK_COMBO_BOX(combo)));
    
    printf("DEBUG: tileNumber=%d, tilesetCount=%d\n", tileNumber, applyCode.tilesetCount);
    printf("DEBUG: tilesetAddress.size=%lu, tilesetWidth.size=%lu, tilesetHeight.size=%lu\n", 
           applyCode.tilesetAddress.size(), applyCode.tilesetWidth.size(), applyCode.tilesetHeight.size());
    fflush(stdout);
    
    // Vérifications de sécurité
    if (tileNumber >= applyCode.tilesetCount) {
        printf("DEBUG: tileNumber >= tilesetCount, returning\n");
        fflush(stdout);
        return;
    }
    
    if (tileNumber >= applyCode.tilesetAddress.size()) {
        printf("DEBUG: tileNumber >= tilesetAddress.size, returning\n");
        fflush(stdout);
        return;
    }
    
    // Si les vecteurs de dimensions sont vides, les initialiser avec des valeurs par défaut
    if (applyCode.tilesetWidth.size() == 0 || applyCode.tilesetHeight.size() == 0) {
        printf("DEBUG: Initializing tileset dimensions with defaults (16x16)\n");
        applyCode.tilesetWidth.clear();
        applyCode.tilesetHeight.clear();
        for (int i = 0; i < applyCode.tilesetCount; i++) {
            applyCode.tilesetWidth.push_back(16);   // 16x16 par défaut (comme dans l'importation)
            applyCode.tilesetHeight.push_back(16);
        }
        fflush(stdout);
    }
    
    if (tileNumber >= applyCode.tilesetWidth.size() || 
        tileNumber >= applyCode.tilesetHeight.size()) {
        printf("DEBUG: tileNumber >= dimension vector size, returning\n");
        fflush(stdout);
        return;
    }
    
    // Vérifier que les dimensions sont valides
    if (applyCode.tilesetWidth[tileNumber] == 0 || applyCode.tilesetHeight[tileNumber] == 0) {
        printf("DEBUG: dimensions are 0 (width=%d, height=%d), returning\n", 
               applyCode.tilesetWidth[tileNumber], applyCode.tilesetHeight[tileNumber]);
        fflush(stdout);
        return;
    }
    
    // Debug : afficher les informations du tileset
    printf("DEBUG Tile %d: addr=0x%X, width=%d, height=%d\n", 
           tileNumber, 
           applyCode.tilesetAddress[tileNumber], 
           applyCode.tilesetWidth[tileNumber], 
           applyCode.tilesetHeight[tileNumber]);
    
    // Créer le pixbuf du tileset sélectionné
    GdkPixbuf *tile_pixbuf = ConvertImageTo24bits(applyCode.tilesetAddress[tileNumber] + 0x2000000, 
                                                   applyCode.tilesetWidth[tileNumber], 
                                                   applyCode.tilesetHeight[tileNumber]);
    
    printf("DEBUG: tile_pixbuf = %p\n", tile_pixbuf);
    
    if (tile_pixbuf != NULL) {
        // Redimensionner l'image pour s'adapter au widget 256x256 en maintenant les proportions
        gint original_width = gdk_pixbuf_get_width(tile_pixbuf);
        gint original_height = gdk_pixbuf_get_height(tile_pixbuf);
        
        // Calculer le facteur de zoom pour s'adapter à 256x256
        gdouble scale_x = 256.0 / original_width;
        gdouble scale_y = 256.0 / original_height;
        gdouble scale = MIN(scale_x, scale_y); // Maintenir les proportions
        
        gint new_width = (gint)(original_width * scale);
        gint new_height = (gint)(original_height * scale);
        
        // Redimensionner le pixbuf
        GdkPixbuf *scaled_pixbuf = gdk_pixbuf_scale_simple(tile_pixbuf, new_width, new_height, GDK_INTERP_NEAREST);
        
        if (scaled_pixbuf != NULL) {
            gtk_image_set_from_pixbuf(GTK_IMAGE(image_widget), scaled_pixbuf);
            g_object_unref(scaled_pixbuf);
        } else {
            gtk_image_set_from_pixbuf(GTK_IMAGE(image_widget), tile_pixbuf);
        }
        
        g_object_unref(tile_pixbuf); // Libérer la référence du pixbuf original
    }
}

// Callback pour changement combobox tile
void on_tile_combo_changed(GtkComboBox *combo, gpointer user_data) {
    TileViewerData *data = static_cast<TileViewerData*>(user_data);
    update_tile_image(GTK_WIDGET(combo), data->image_widget);
}

void
on_tilesets_more_clicked(GtkButton *button, gpointer user_data)
{
    (void)button;
    printf("DEBUG: on_tilesets_more_clicked called\n");
    fflush(stdout);
    GtkWindow *parent_window = GTK_WINDOW(user_data);
    open_tile_viewer(parent_window);
}

void
open_tile_viewer(GtkWindow *parent_window)
{
    printf("DEBUG: open_tile_viewer called, tilesetCount=%d, tilesetPath.size=%lu\n", 
           applyCode.tilesetCount, applyCode.tilesetPath.size());
    fflush(stdout);
    
    // Vérifier qu'il y a des tilesets à afficher
    if (applyCode.tilesetCount == 0 || applyCode.tilesetPath.empty()) {
        printf("DEBUG: No tilesets to display, returning\n");
        fflush(stdout);
        return;
    }
    
    GtkWidget *dialog = gtk_dialog_new_with_buttons("View Tileset",
                                                    parent_window,
                                                    GTK_DIALOG_MODAL,
                                                    "_Close",
                                                    GTK_RESPONSE_CLOSE,
                                                    NULL);

    gtk_window_set_default_size(GTK_WINDOW(dialog), 300, 400);

    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(container), 20);

    // Label et combobox pour sélectionner le tileset
    GtkWidget *tile_label = gtk_label_new("Select Tileset:");
    gtk_widget_set_halign(tile_label, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(container), tile_label, FALSE, FALSE, 0);

    GtkWidget *tile_combo = gtk_combo_box_text_new();
    for (int i = 1; i <= applyCode.tilesetCount; i++) {
        std::string tile_number = std::to_string(i);
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(tile_combo), tile_number.c_str());
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(tile_combo), 0); // Sélectionner le premier par défaut
    gtk_box_pack_start(GTK_BOX(container), tile_combo, FALSE, FALSE, 0);

    // Zone pour afficher l'image (256x256 maximum)
    GtkWidget *image_frame = gtk_frame_new("Tileset Image");
    GtkWidget *image_scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(image_scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(image_scrolled, 256, 256);
    
    GtkWidget *image_widget = gtk_image_new();
    gtk_container_add(GTK_CONTAINER(image_scrolled), image_widget);
    gtk_container_add(GTK_CONTAINER(image_frame), image_scrolled);
    gtk_box_pack_start(GTK_BOX(container), image_frame, TRUE, TRUE, 0);

    // Créer la structure pour le callback
    TileViewerData *viewer_data = g_new(TileViewerData, 1);
    viewer_data->combo = tile_combo;
    viewer_data->image_widget = image_widget;

    // Connecter le signal changed de la combobox
    g_signal_connect(G_OBJECT(tile_combo), "changed", G_CALLBACK(on_tile_combo_changed), viewer_data);
    
    // Connecter la destruction du dialog pour libérer la mémoire
    g_signal_connect_swapped(G_OBJECT(dialog), "destroy", G_CALLBACK(g_free), viewer_data);

    // Charger l'image initiale
    update_tile_image(tile_combo, image_widget);

    gtk_container_add(GTK_CONTAINER(content_area), container);
    gtk_widget_show_all(dialog);

    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}
