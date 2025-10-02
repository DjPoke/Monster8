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
                 << applyCode.spriteCount << '\n'
                 << applyCode.tilesetCount << '\n'
                 << applyCode.soundCount << '\n'
                 << applyCode.musicCount << '\n'
                 << applyCode.nextSpriteAddr << '\n'
                 << applyCode.nextTilesetAddr << '\n'
                 << applyCode.nextSoundAddr << '\n'
                 << applyCode.nextMusicAddr << '\n';

    project_file << applyCode.spritePath.size() << '\n';

    for(uint32_t i = 0; i < applyCode.spritePath.size(); i++) {
        project_file << applyCode.spritePath[i] << '\n'
                     << applyCode.spriteAddress[i] << '\n'
                     << applyCode.spriteLength[i] << '\n'
                     << applyCode.spriteWidth[i] << '\n'
                     << applyCode.spriteHeight[i] << '\n';
    }

    project_file << applyCode.tilesetPath.size() << '\n';
    
    for(uint32_t i = 0; i < applyCode.tilesetPath.size(); i++) {
        project_file << applyCode.tilesetPath[i] << '\n'
                     << applyCode.tilesetAddress[i] << '\n'
                     << applyCode.tilesetLength[i] << '\n';
    }

    project_file << applyCode.soundPath.size() << '\n';
    
    for(uint32_t i = 0; i < applyCode.soundPath.size(); i++) {
        project_file << applyCode.soundPath[i] << '\n'
                     << applyCode.soundAddress[i] << '\n'
                     << applyCode.soundLength[i] << '\n';
    }

    project_file << applyCode.musicPath.size() << '\n';
    
    for(uint32_t i = 0; i < applyCode.musicPath.size(); i++) {
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
