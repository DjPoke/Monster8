#ifndef GTK_P1_FUNCTIONS_HPP
#define GTK_P1_FUNCTIONS_HPP

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <cstdint>
#include <string>
#include <vector>

#include "code.hpp"
#include "gtk_p2.hpp"

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

// File operations
void OpenProject(GtkWindow *parent_window, GtkWidget *text_view);
void SaveProject(std::string filename);
std::string SaveProjectAs(GtkWindow *parent_window);
void SaveROM(std::string filename);
void LoadROM(std::string filename);
std::string SaveROMAs(GtkWindow *parent_window);

// Build function
bool Build(void);

#endif // GTK_FUNCTIONS_HPP