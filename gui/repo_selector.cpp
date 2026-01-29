#include <algorithm>
#include <filesystem>
#include <ranges>
#include <wx/button.h>
#include <wx/event.h>
#include <wx/popupwin.h>
#include <wx/scrolwin.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>

#include "app_settings.hpp"
#include "git/utils/repo_name_extractor.hpp"
#include "git/unique_libgit_ptr.hpp"
#include "repo_selector.hpp"

namespace fons::gui
{

    struct to_lower_fn
    {
        char operator()(unsigned char c) const noexcept
        {
            return static_cast<char>(std::tolower(c));
        }
    };

    inline constexpr to_lower_fn to_lower{};

    repo_selector::repo_selector(wxWindow *parent, app_settings &settings) : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE)
    {
        subscribed_settings = &settings;
        
        // Set up the main control layout
        auto *main_sizer = new wxBoxSizer(wxHORIZONTAL);
        
        // Create the active repository name text
        active_name_text = new wxStaticText(this, wxID_ANY, "No repository selected", wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE);
        wxFont bold_font = active_name_text->GetFont();
        bold_font.SetWeight(wxFONTWEIGHT_BOLD);
        active_name_text->SetFont(bold_font);
        
        // Create dropdown button
        dropdown_button = new wxButton(this, wxID_ANY, "▼", wxDefaultPosition, wxSize(24, -1), wxBU_EXACTFIT);
        dropdown_button->SetMinSize(wxSize(24, -1));
        
        main_sizer->Add(active_name_text, wxSizerFlags(1).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 2));
        main_sizer->Add(dropdown_button, wxSizerFlags().FixedMinSize().Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 2));
        
        SetSizer(main_sizer);
        
        // Bind events
        dropdown_button->Bind(wxEVT_BUTTON, &repo_selector::on_dropdown_button_click, this);
        //Bind(wxEVT_KILL_FOCUS, &repo_selector::on_kill_focus, this);
        
        // Initialize repositories from settings
        refresh_repository_list();
        
        // Set the active repository if one exists
        if (!settings.active_repo.empty())
        {
            current_repo_path = settings.active_repo;
            update_active_display();
        }
        
        // Subscribe to settings changes
        settings.subscribe(this);
    }

    repo_selector::~repo_selector()
    {
        if (subscribed_settings)
        {
            subscribed_settings->unsubscribe(this);
        }
        
        if (popup_window)
        {
            popup_window->Destroy();
        }
    }

    void repo_selector::on_repo_found(std::string_view found_repo)
    {
        std::string repo_path(found_repo);
        
        // Check if we already have this repository
        if (std::ranges::any_of(repositories, [&](const auto& repo) { return repo.path == repo_path; }))
        {
            return;
        }
        
        // Add the new repository with its display name
        repository_info repo;
        repo.path = repo_path;
        repo.display_name = get_display_name_for_path(repo_path);
        
        repositories.push_back(repo);
        
        // Update filtered list if dropdown is open
        if (popup_window && popup_window->IsShown())
        {
            apply_filter(current_filter_text);
        }
    }

    void repo_selector::refresh_repository_list()
    {
        repositories.clear();
        
        for (const std::string& repo_path : subscribed_settings->repos)
        {
            repository_info repo;
            repo.path = repo_path;
            repo.display_name = get_display_name_for_path(repo_path);
            repositories.push_back(repo);
        }
        
        // Sort repositories by display name, then by path
        std::ranges::sort(repositories, [](const repository_info& a, const repository_info& b) {
            if (a.display_name != b.display_name)
                return a.display_name < b.display_name;
            return a.path < b.path;
        });
        
        filtered_repositories = repositories;
    }

    void repo_selector::update_active_display()
    {
        if (current_repo_path.empty())
        {
            active_name_text->SetLabel("No repository selected");
            return;
        }
        
        std::string display_name = get_display_name_for_path(current_repo_path);
        active_name_text->SetLabel(wxString(display_name));
    }

    std::string repo_selector::get_display_name_for_path(const std::string& path)
    {
        // Check cache first
        auto it = name_cache.find(path);
        if (it != name_cache.end())
        {
            return it->second;
        }
        
        // Extract the display name using our utility
        std::string display_name = fons::git::utils::get_repo_display_name(path);
        
        // Cache the result
        name_cache[path] = display_name;
        
        return display_name;
    }

    void repo_selector::show_dropdown()
    {
        if (popup_window)
        {
            hide_dropdown();
            return;
        }
        
        create_popup_window();
        
        if (popup_window)
        {
            // 1. Get the screen position of THIS control (the top-left corner)
            wxPoint screen_pos = this->ClientToScreen(wxPoint(0, 0));

            // 2. Get the top-level window (the main App frame) to find the bottom boundary
            wxWindow *top_parent = wxGetTopLevelParent(this);
            wxRect app_rect = top_parent->GetScreenRect();
            int app_bottom = app_rect.y + app_rect.height;

            // 3. Calculate dimensions
            int width = GetSize().GetWidth();
            // Height = (Bottom of App) - (Top of this control)
            int height = app_bottom - screen_pos.y;

            // 4. Set the size and position
            popup_window->SetSize(width, height);
            popup_window->Move(screen_pos);

            // Popup() usually takes a focus window; we pass the filter_input so user can start typing immediately
            //popup_window->Position(screen_pos, wxSize(width, height));
            popup_window->Popup(filter_input);
        }
    }

    void repo_selector::hide_dropdown()
    {
        if (popup_window)
        {
            popup_window->Dismiss();
            popup_window->Destroy();
            popup_window = nullptr;
        }
    }

    void repo_selector::create_popup_window()
    {
        popup_window = new wxPopupTransientWindow(this, wxBORDER_SIMPLE | wxPU_CONTAINS_CONTROLS);

        auto *popup_sizer = new wxBoxSizer(wxVERTICAL);
        
        // Create filter input
        create_filter_input();
        if (filter_input)
        {
            popup_sizer->Add(filter_input, wxSizerFlags().Expand().Border(wxALL, 5));
        }
        
        // Create repository list
        create_repository_list();
        if (popup_content)
        {
            popup_sizer->Add(popup_content, wxSizerFlags(1).Expand().Border(wxALL, 5));
        }
        
        popup_window->SetSizer(popup_sizer);

        // Initialize the repository items
        update_repository_items();

        popup_window->Layout();
    }

    void repo_selector::create_filter_input()
    {
        filter_input = new wxTextCtrl(popup_window, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize);
        filter_input->SetHint("Type to filter repositories...");
        filter_input->Bind(wxEVT_TEXT, &repo_selector::on_filter_text_changed, this);
    }

    void repo_selector::create_repository_list()
    {
        popup_content = new wxScrolledWindow(popup_window, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL);
        popup_content->SetMinSize(wxSize(300, 150));
        
        auto *content_sizer = new wxBoxSizer(wxVERTICAL);
        popup_content->SetSizer(content_sizer);
        
        // Enable scrolling
        popup_content->SetScrollRate(0, 20);
    }

    void repo_selector::update_repository_items()
    {
        if (!popup_content)
            return;
     
        popup_content->Freeze();

        // Clear existing items
        popup_content->GetSizer()->Clear(true);
        
        // Add items for filtered repositories
        for (const auto& repo : filtered_repositories)
        {
            auto *item = new repo_list_item(popup_content, repo, this);
            popup_content->GetSizer()->Add(item, wxSizerFlags().Expand().Border(wxALL, 1));
        }
        
        popup_content->FitInside();
        popup_content->Layout();
        popup_content->Thaw();
    }

    void repo_selector::apply_filter(const std::string& filter)
    {
        current_filter_text = filter;
        
        if (filter.empty())
        {
            filtered_repositories = repositories;
        }
        else
        {
            filtered_repositories.clear();
            std::string lower_filter = filter;
            std::ranges::transform(lower_filter, lower_filter.begin(), to_lower);
            
            for (const auto& repo : repositories)
            {
                if (matches_filter(repo, lower_filter))
                {
                    filtered_repositories.push_back(repo);
                }
            }
        }
        
        update_repository_items();
    }

    bool repo_selector::matches_filter(const repository_info& repo, const std::string& filter)
    {
        std::string lower_name = repo.display_name;
        std::string lower_path = repo.path;
        
        std::ranges::transform(lower_name, lower_name.begin(), to_lower);
        std::ranges::transform(lower_path, lower_path.begin(), to_lower);
        
        return lower_name.find(filter) != std::string::npos || 
               lower_path.find(filter) != std::string::npos;
    }

    void repo_selector::select_repository(const std::string& path)
    {
        /*
        if (path == current_repo_path)
        {
            hide_dropdownD();
            return;
        }
        */
        current_repo_path = path;
        update_active_display();
        
        // Update settings
        subscribed_settings->active_repo = path;
        
        // Notify observers of the change by simulating a combo box selection event
        wxCommandEvent select_event(wxEVT_COMBOBOX);
        select_event.SetString(wxString(path));
        subscribed_settings->on_repo_select(select_event);
        
        hide_dropdown();
    }

    void repo_selector::on_dropdown_button_click([[maybe_unused]] wxCommandEvent &event)
    {
        show_dropdown();
    }

    void repo_selector::on_filter_text_changed([[maybe_unused]] wxCommandEvent &event)
    {
        if (filter_input)
        {
            apply_filter(filter_input->GetValue().ToStdString());
        }
    }

    void repo_selector::on_popup_dismiss([[maybe_unused]] wxCommandEvent &event)
    {
        hide_dropdown();
    }

    void repo_selector::on_kill_focus(wxFocusEvent &event)
    {
        // Small delay to allow click events to be processed
        CallAfter([this]() {
            if (popup_window && popup_window->IsShown())
            {
                hide_dropdown();
            }
        });
        
        event.Skip();
    }

    std::string repo_selector::normalize_path_for_display(const std::string& path)
    {
        std::string normalized = path;
        std::ranges::replace(normalized, '\\', '/');
        if (normalized.length() > 4 && normalized.ends_with(".git"))
        {
            normalized.erase(normalized.end() - 4, normalized.end());
        }
        return normalized;
    }

    // repo_list_item implementation
    repo_list_item::repo_list_item(wxWindow *parent, const repo_selector::repository_info &info, repo_selector *selector)
        : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE), repo_info(info), parent_selector(selector)
    {
        SetBackgroundColour(wxColour(255, 255, 255));
        
        auto *sizer = new wxBoxSizer(wxVERTICAL);
        
        // Create name text (bold)
        name_text = new wxStaticText(this, wxID_ANY, info.display_name, wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE);
        wxFont bold_font = name_text->GetFont();
        bold_font.SetWeight(wxFONTWEIGHT_BOLD);
        bold_font.SetPointSize(bold_font.GetPointSize());
        name_text->SetFont(bold_font);
        name_text->SetForegroundColour(wxColour(0, 0, 0));
        
        // Create path text (smaller, gray)
        path_text = new wxStaticText(this, wxID_ANY, parent_selector->normalize_path_for_display(info.path), wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE);
        wxFont small_font = path_text->GetFont();
        small_font.SetPointSize(small_font.GetPointSize() - 1);
        path_text->SetFont(small_font);
        path_text->SetForegroundColour(wxColour(128, 128, 128));
        
        sizer->Add(name_text, wxSizerFlags().Align(wxALIGN_LEFT).Border(wxALL, 2));
        sizer->Add(path_text, wxSizerFlags().Align(wxALIGN_LEFT).Border(wxLEFT | wxRIGHT | wxBOTTOM, 2));
        
        SetSizer(sizer);
        
        // Bind events
        Bind(wxEVT_ENTER_WINDOW, &repo_list_item::on_mouse_enter, this);
        Bind(wxEVT_LEAVE_WINDOW, &repo_list_item::on_mouse_leave, this);
        Bind(wxEVT_LEFT_UP, &repo_list_item::on_click, this);
        name_text->Bind(wxEVT_LEFT_UP, &repo_list_item::on_click, this);
        path_text->Bind(wxEVT_LEFT_UP, &repo_list_item::on_click, this);
        
        SetMinSize(wxSize(-1, 40));
    }

    void repo_list_item::on_mouse_enter(wxMouseEvent &event)
    {
        update_colors(true);
        SetCursor(wxCursor(wxCURSOR_HAND));
        event.Skip();
    }

    void repo_list_item::on_mouse_leave(wxMouseEvent &event)
    {
        update_colors(false);
        SetCursor(wxCursor(wxCURSOR_ARROW));
        event.Skip();
    }

    void repo_list_item::on_click(wxMouseEvent &event)
    {
        if (parent_selector)
        {
            parent_selector->select_repository(repo_info.path);
        }
        event.Skip();
    }

    void repo_list_item::update_colors(bool is_hovered)
    {
        if (is_hovered)
        {
            SetBackgroundColour(wxColour(240, 240, 240));
        }
        else
        {
            SetBackgroundColour(wxColour(255, 255, 255));
        }
        
        if (name_text)
            name_text->SetBackgroundColour(GetBackgroundColour());
        if (path_text)
            path_text->SetBackgroundColour(GetBackgroundColour());
        
        Refresh();
    }

} // namespace fons::gui