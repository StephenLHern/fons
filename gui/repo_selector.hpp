#pragma once

#include "gui/fons_wx.hpp"
#include "settings_observer.hpp"
#include <wx/popupwin.h>
#include <wx/scrolwin.h>
#include <wx/textctrl.h>
#include <vector>
#include <unordered_map>

namespace fons
{
    class app_settings;
}

namespace fons::gui
{
    class repo_list_item;

    class repo_selector : public wxPanel, private settings_observer
    {
      public:
        repo_selector(wxWindow *parent, fons::app_settings &settings);
        repo_selector(const repo_selector& other) = delete;
        repo_selector &operator=(const repo_selector &other) = delete;
        ~repo_selector() override;
        void on_repo_found(std::string_view found_repo) override;

      private:
        struct repository_info
        {
            std::string path;          // File path (primary key)
            std::string display_name;  // Extracted name for display
        };

        std::vector<repository_info> repositories;
        std::vector<repository_info> filtered_repositories;
        std::unordered_map<std::string, std::string> name_cache; // path -> display_name cache
        std::string current_repo_path;
        std::string current_filter_text;

        // UI elements in main control
        wxStaticText *active_name_text;
        wxButton *dropdown_button;

        // Popup window elements
        wxPopupTransientWindow *popup_window = nullptr;
        wxScrolledWindow *popup_content = nullptr;
        wxTextCtrl *filter_input = nullptr;
        wxPanel *filter_panel = nullptr;

        fons::app_settings *subscribed_settings = nullptr;

        // Core functionality
        void show_dropdown();
        void hide_dropdown();
        void select_repository(const std::string& path);
        void apply_filter(const std::string& filter);
        void refresh_repository_list();
        void update_active_display();
        void update_filtered_list();

        // UI creation and management
        void create_popup_window();
        void create_filter_input();
        void create_repository_list();
        void update_repository_items();

        // Helper methods
        std::string get_display_name_for_path(const std::string& path);
        std::string normalize_path_for_display(const std::string& path);
        bool matches_filter(const repository_info& repo, const std::string& filter);

        // Event handlers
        void on_dropdown_button_click([[maybe_unused]] wxCommandEvent &event);
        void on_filter_text_changed([[maybe_unused]] wxCommandEvent &event);
        void on_popup_dismiss([[maybe_unused]] wxCommandEvent &event);
        void on_kill_focus(wxFocusEvent &event);

        // Friends
        friend class repo_list_item;
    };

    class repo_list_item : public wxPanel
    {
      public:
        repo_list_item(wxWindow *parent, const repo_selector::repository_info &info, repo_selector *selector);
        
      private:
        repo_selector::repository_info repo_info;
        repo_selector *parent_selector;
        wxStaticText *name_text;
        wxStaticText *path_text;
        
        void on_mouse_enter(wxMouseEvent &event);
        void on_mouse_leave(wxMouseEvent &event);
        void on_click(wxMouseEvent &event);
        void update_colors(bool is_hovered);
    };

} // namespace fons::gui