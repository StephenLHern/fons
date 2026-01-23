#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>

#include "diff_control.hpp"
#include "git2/diff.h"
#include <wx/artprov.h>

#ifdef _WIN32
#include <windows.h>
#include <shellapi.h>
#else
#include <cstdlib>
#endif

namespace fons::gui
{
    diff_control::diff_control(wxWindow *parent)
        : wxPanel(parent), current_algorithm(diff_algorithm::meyers), left_editable(true), right_editable(true)
    {
        create_ui();
    }

    diff_control::~diff_control() = default;

    void diff_control::create_ui()
    {
        auto main_sizer = new wxBoxSizer(wxVERTICAL);

        // Create collapsible controls panel
        controls_panel = new wxCollapsiblePane(this, wxID_ANY, "Diff Controls");
        auto controls_pane = controls_panel->GetPane();
        auto controls_sizer = new wxBoxSizer(wxVERTICAL);

        // Algorithm selection with icons
        auto algo_label = new wxStaticText(controls_pane, wxID_ANY, "Diff Algorithm:");
        controls_sizer->Add(algo_label, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);

        auto algo_sizer = new wxBoxSizer(wxHORIZONTAL);
        
        // Create placeholder icons
        auto icons = create_algorithm_icons();
        
        // Meyers algorithm radio with icon
        meyers_icon = new wxStaticBitmap(controls_pane, wxID_ANY, icons[0]);
        meyers_radio = new wxRadioButton(controls_pane, wxID_ANY, "Meyers", wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
        auto meyers_sizer = new wxBoxSizer(wxVERTICAL);
        meyers_sizer->Add(meyers_icon, 0, wxALIGN_CENTER | wxBOTTOM, 2);
        meyers_sizer->Add(meyers_radio, 0, wxALIGN_CENTER);
        algo_sizer->Add(meyers_sizer, 0, wxALL, 5);
        
        // Bentley-McIlroy algorithm radio with icon
        bentley_mcilroy_icon = new wxStaticBitmap(controls_pane, wxID_ANY, icons[1]);
        bentley_mcilroy_radio = new wxRadioButton(controls_pane, wxID_ANY, "Bentley-McIlroy");
        auto bentley_sizer = new wxBoxSizer(wxVERTICAL);
        bentley_sizer->Add(bentley_mcilroy_icon, 0, wxALIGN_CENTER | wxBOTTOM, 2);
        bentley_sizer->Add(bentley_mcilroy_radio, 0, wxALIGN_CENTER);
        algo_sizer->Add(bentley_sizer, 0, wxALL, 5);
        
        // XDelta algorithm radio with icon
        xdelta_icon = new wxStaticBitmap(controls_pane, wxID_ANY, icons[2]);
        xdelta_radio = new wxRadioButton(controls_pane, wxID_ANY, "XDelta");
        auto xdelta_sizer = new wxBoxSizer(wxVERTICAL);
        xdelta_sizer->Add(xdelta_icon, 0, wxALIGN_CENTER | wxBOTTOM, 2);
        xdelta_sizer->Add(xdelta_radio, 0, wxALIGN_CENTER);
        algo_sizer->Add(xdelta_sizer, 0, wxALL, 5);
        
        // BSDiff algorithm radio with icon
        bsdiff_icon = new wxStaticBitmap(controls_pane, wxID_ANY, icons[3]);
        bsdiff_radio = new wxRadioButton(controls_pane, wxID_ANY, "BSDiff");
        auto bsdiff_sizer = new wxBoxSizer(wxVERTICAL);
        bsdiff_sizer->Add(bsdiff_icon, 0, wxALIGN_CENTER | wxBOTTOM, 2);
        bsdiff_sizer->Add(bsdiff_radio, 0, wxALIGN_CENTER);
        algo_sizer->Add(bsdiff_sizer, 0, wxALL, 5);

        // Set default selection and bind events
        meyers_radio->SetValue(true);
        
        meyers_radio->Bind(wxEVT_RADIOBUTTON, [this](wxCommandEvent&) { 
            set_diff_algorithm(diff_algorithm::meyers); 
            update_diff(); 
        });
        bentley_mcilroy_radio->Bind(wxEVT_RADIOBUTTON, [this](wxCommandEvent&) { 
            set_diff_algorithm(diff_algorithm::bentley_mcilroy); 
            update_diff(); 
        });
        xdelta_radio->Bind(wxEVT_RADIOBUTTON, [this](wxCommandEvent&) { 
            set_diff_algorithm(diff_algorithm::xdelta); 
            update_diff(); 
        });
        bsdiff_radio->Bind(wxEVT_RADIOBUTTON, [this](wxCommandEvent&) { 
            set_diff_algorithm(diff_algorithm::bsdiff); 
            update_diff(); 
        });

        controls_sizer->Add(algo_sizer, 0, wxEXPAND | wxALL, 5);
        controls_pane->SetSizer(controls_sizer);
        main_sizer->Add(controls_panel, 0, wxEXPAND | wxALL, 5);

        // Create scrollable content window
        content_scroll_window = new wxScrolledWindow(this, wxID_ANY);
        content_scroll_window->SetScrollRate(10, 10);
        
        auto content_sizer = new wxBoxSizer(wxVERTICAL);

        // Left file section with path and buttons
        auto left_section = new wxBoxSizer(wxVERTICAL);
        auto left_header = new wxBoxSizer(wxHORIZONTAL);
        left_path_text = new wxStaticText(content_scroll_window, wxID_ANY, "");
        left_path_text->SetForegroundColour(wxColour(100, 100, 100));
        left_header->Add(left_path_text, 1, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
        
        // Create folder and file open bitmaps
        wxBitmap folder_bitmap = wxArtProvider::GetBitmap(wxART_FOLDER, wxART_MENU, wxSize(16, 16));
        wxBitmap file_bitmap = wxArtProvider::GetBitmap(wxART_NORMAL_FILE, wxART_MENU, wxSize(16, 16));
        
        left_folder_button = new wxBitmapButton(content_scroll_window, wxID_ANY, folder_bitmap);
        left_folder_button->SetToolTip("Open containing folder");
        left_folder_button->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { open_containing_folder(left_path); });
        left_header->Add(left_folder_button, 0, wxRIGHT, 2);
        
        left_file_button = new wxBitmapButton(content_scroll_window, wxID_ANY, file_bitmap);
        left_file_button->SetToolTip("Open file in default application");
        left_file_button->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { open_file_in_os(left_path); });
        left_header->Add(left_file_button, 0);
        
        left_section->Add(left_header, 0, wxEXPAND | wxALL, 2);
        
        left_text = new wxStyledTextCtrl(content_scroll_window, wxID_ANY);
        left_text->SetUseHorizontalScrollBar(false);
        left_text->SetUseVerticalScrollBar(false);
        left_text->SetMarginWidth(1, 50);
        left_text->SetMarginType(1, wxSTC_MARGIN_NUMBER);
        left_text->SetMinSize(wxSize(400, 300));
        left_section->Add(left_text, 1, wxEXPAND | wxALL, 2);

        // Right file section with path and buttons
        auto right_section = new wxBoxSizer(wxVERTICAL);
        auto right_header = new wxBoxSizer(wxHORIZONTAL);
        right_path_text = new wxStaticText(content_scroll_window, wxID_ANY, "");
        right_path_text->SetForegroundColour(wxColour(100, 100, 100));
        right_header->Add(right_path_text, 1, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
        
        right_folder_button = new wxBitmapButton(content_scroll_window, wxID_ANY, folder_bitmap);
        right_folder_button->SetToolTip("Open containing folder");
        right_folder_button->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { open_containing_folder(right_path); });
        right_header->Add(right_folder_button, 0, wxRIGHT, 2);
        
        right_file_button = new wxBitmapButton(content_scroll_window, wxID_ANY, file_bitmap);
        right_file_button->SetToolTip("Open file in default application");
        right_file_button->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { open_file_in_os(right_path); });
        right_header->Add(right_file_button, 0);
        
        right_section->Add(right_header, 0, wxEXPAND | wxALL, 2);
        
        right_text = new wxStyledTextCtrl(content_scroll_window, wxID_ANY);
        right_text->SetUseHorizontalScrollBar(false);
        right_text->SetUseVerticalScrollBar(false);
        right_text->SetMarginWidth(1, 50);
        right_text->SetMarginType(1, wxSTC_MARGIN_NUMBER);
        right_text->SetMinSize(wxSize(400, 300));
        right_section->Add(right_text, 1, wxEXPAND | wxALL, 2);

        // Add both sections to content sizer
        auto files_sizer = new wxBoxSizer(wxHORIZONTAL);
        files_sizer->Add(left_section, 1, wxEXPAND | wxALL, 5);
        files_sizer->Add(right_section, 1, wxEXPAND | wxALL, 5);
        
        content_sizer->Add(files_sizer, 1, wxEXPAND);
        content_scroll_window->SetSizer(content_sizer);
        
        main_sizer->Add(content_scroll_window, 1, wxEXPAND | wxALL, 5);
        
        SetSizerAndFit(main_sizer);
    }

    void diff_control::load_files(std::string_view left_path_input, std::string_view right_path_input)
    {
        left_path = left_path_input;
        right_path = right_path_input;

        std::string left_content;
        std::string right_content;

        std::ifstream left_file(std::string(left_path), std::ios::binary);
        if (left_file)
        {
            std::stringstream left_buffer;
            left_buffer << left_file.rdbuf();
            left_content = left_buffer.str();
        }

        std::ifstream right_file(std::string(right_path), std::ios::binary);
        if (right_file)
        {
            std::stringstream right_buffer;
            right_buffer << right_file.rdbuf();
            right_content = right_buffer.str();
        }

        load_content(left_content, right_content);

        if (!left_alias.empty())
            left_path_text->SetLabel(wxString(left_alias));
        else
            left_path_text->SetLabel(wxString(left_path));

        if (!right_alias.empty())
            right_path_text->SetLabel(wxString(right_alias));
        else
            right_path_text->SetLabel(wxString(right_path));
    }

    void diff_control::load_content(std::string_view left_content_input, std::string_view right_content_input)
    {
        left_text->SetText(wxString(left_content_input.data()));
        right_text->SetText(wxString(right_content_input.data()));
        update_diff();
    }

    void diff_control::set_left_alias(std::string_view alias)
    {
        left_alias = alias;
        if (!left_alias.empty())
            left_path_text->SetLabel(wxString(left_alias));
    }

    void diff_control::set_right_alias(std::string_view alias)
    {
        right_alias = alias;
        if (!right_alias.empty())
            right_path_text->SetLabel(wxString(right_alias));
    }

    void diff_control::set_left_editable(bool editable)
    {
        left_editable = editable;
        left_text->SetReadOnly(!editable);
    }

    void diff_control::set_right_editable(bool editable)
    {
        right_editable = editable;
        right_text->SetReadOnly(!editable);
    }

    void diff_control::set_diff_algorithm(diff_algorithm algo)
    {
        current_algorithm = algo;
    }

    void diff_control::set_diff_colors(const diff_colors &colors)
    {
        current_colors = colors;
        update_diff();
    }

    void diff_control::load_from_git_diff(const git::file_diff &diff)
    {
        left_path = diff.old_file_path;
        right_path = diff.new_file_path;

        load_content(diff.old_file_content, diff.new_file_conent);

        if (!left_alias.empty())
            left_path_text->SetLabel(wxString(left_alias));
        else if (!left_path.empty())
            left_path_text->SetLabel(wxString(left_path));

        if (!right_alias.empty())
            right_path_text->SetLabel(wxString(right_alias));
        else if (!right_path.empty())
            right_path_text->SetLabel(wxString(right_path));
    }

    void diff_control::update_diff()
    {
        if (current_algorithm == diff_algorithm::meyers)
        {
            std::string left_str = left_text->GetText().ToStdString();
            std::string right_str = right_text->GetText().ToStdString();

            auto left_lines = split_lines(left_str);
            auto right_lines = split_lines(right_str);

            std::vector<int> left_statuses(left_lines.size(), 0);
            std::vector<int> right_statuses(right_lines.size(), 0);

            compute_line_diff(left_lines, right_lines, left_statuses, right_statuses);

            left_text->Freeze();
            right_text->Freeze();

            left_text->ClearAll();
            right_text->ClearAll();

            apply_diff_lines(left_lines, left_text, left_statuses);
            apply_diff_lines(right_lines, right_text, right_statuses);

            left_text->Thaw();
            right_text->Thaw();
        }
        else if (current_algorithm == diff_algorithm::bentley_mcilroy || current_algorithm == diff_algorithm::xdelta ||
                 current_algorithm == diff_algorithm::bsdiff)
        {

            std::string left_str = left_text->GetText().ToStdString();
            std::string right_str = right_text->GetText().ToStdString();

            auto left_lines = split_lines(left_str);
            auto right_lines = split_lines(right_str);

            std::vector<int> left_statuses(left_lines.size(), 0);
            std::vector<int> right_statuses(right_lines.size(), 0);

            compute_line_diff(left_lines, right_lines, left_statuses, right_statuses);

            left_text->Freeze();
            right_text->Freeze();

            left_text->ClearAll();
            right_text->ClearAll();

            apply_diff_lines(left_lines, left_text, left_statuses);
            apply_diff_lines(right_lines, right_text, right_statuses);

            left_text->Thaw();
            right_text->Thaw();
        }
    }

    void diff_control::apply_diff_lines(const std::vector<std::string> &lines, wxStyledTextCtrl *control,
                                        const std::vector<int> &line_statuses)
    {
        for (size_t i = 0; i < lines.size(); i++)
        {
            control->AppendText(wxString(lines[i]));
            if (i < lines.size() - 1)
                control->AppendText("\n");

            if (line_statuses[i] == 1)
            {
                control->StyleSetBackground(2, current_colors.added_lines);
                control->StartStyling(control->PositionFromLine(static_cast<int>(i)));
                control->SetStyling(static_cast<int>(lines[i].length()), 2);
            }
            else if (line_statuses[i] == -1)
            {
                control->StyleSetBackground(3, current_colors.removed_lines);
                control->StartStyling(control->PositionFromLine(static_cast<int>(i)));
                control->SetStyling(static_cast<int>(lines[i].length()), 3);
            }
            else if (line_statuses[i] == 2)
            {
                control->StyleSetBackground(4, current_colors.changed_lines);
                control->StartStyling(control->PositionFromLine(static_cast<int>(i)));
                control->SetStyling(static_cast<int>(lines[i].length()), 4);
            }
        }
    }

    size_t diff_control::compute_line_diff(const std::vector<std::string> &left_lines, const std::vector<std::string> &right_lines,
                                        std::vector<int> &left_statuses, std::vector<int> &right_statuses)
    {
        std::vector<std::pair<size_t, size_t>> lcs_pairs;
        size_t lcs_len = compute_lcs(left_lines, right_lines, lcs_pairs);

        for (size_t i = 0; i < left_lines.size(); i++)
        {
            bool is_in_lcs = false;
            for (const auto &pair : lcs_pairs)
            {
                if (static_cast<size_t>(pair.first) == i)
                {
                    is_in_lcs = true;
                    break;
                }
            }
            if (!is_in_lcs)
            {
                left_statuses[i] = -1;
            }
        }

        for (size_t i = 0; i < right_lines.size(); i++)
        {
            bool is_in_lcs = false;
            for (const auto &pair : lcs_pairs)
            {
                if (static_cast<size_t>(pair.second) == i)
                {
                    is_in_lcs = true;
                    break;
                }
            }
            if (!is_in_lcs)
            {
                right_statuses[i] = 1;
            }
        }

        return lcs_len;
    }

    size_t diff_control::lcs_length(const std::string &a, const std::string &b)
    {
        size_t m = a.length();
        size_t n = b.length();
        std::vector<std::vector<size_t>> dp(m + 1, std::vector<size_t>(n + 1, 0));

        for (size_t i = 1; i <= m; i++)
        {
            for (size_t j = 1; j <= n; j++)
            {
                if (a[i - 1] == b[j - 1])
                {
                    dp[i][j] = dp[i - 1][j - 1] + 1;
                }
                else
                {
                    dp[i][j] = std::max(dp[i - 1][j], dp[i][j - 1]);
                }
            }
        }

        return dp[m][n];
    }

    size_t diff_control::compute_lcs(const std::vector<std::string> &left_lines, const std::vector<std::string> &right_lines,
                                     std::vector<std::pair<size_t, size_t>> &lcs)
    {
        size_t m = left_lines.size();
        size_t n = right_lines.size();

        std::vector<std::vector<size_t>> dp(m + 1, std::vector<size_t>(n + 1, 0));

        for (size_t i = 1; i <= m; i++)
        {
            for (size_t j = 1; j <= n; j++)
            {
                if (left_lines[i - 1] == right_lines[j - 1])
                {
                    dp[i][j] = dp[i - 1][j - 1] + 1;
                }
                else
                {
                    dp[i][j] = std::max(dp[i - 1][j], dp[i][j - 1]);
                }
            }
        }

        size_t i = m, j = n;
        while (i > 0 && j > 0)
        {
            if (left_lines[i - 1] == right_lines[j - 1])
            {
                lcs.push_back({i - 1, j - 1});
                i--;
                j--;
            }
            else if (dp[i - 1][j] > dp[i][j - 1])
            {
                i--;
            }
            else
            {
                j--;
            }
        }

        std::reverse(lcs.begin(), lcs.end());

        return lcs.size();
    }

    std::vector<std::string> diff_control::split_lines(const std::string &content)
    {
        std::vector<std::string> lines;
        std::istringstream stream(content);
        std::string line;

        while (std::getline(stream, line))
        {
            lines.push_back(line);
        }

        if (!content.empty() && content.back() == '\n')
        {
            lines.push_back("");
        }

        return lines;
    }

    std::vector<wxBitmap> diff_control::create_algorithm_icons()
    {
        std::vector<wxBitmap> icons;
        
        // Create placeholder 32x32 bitmaps with different colors for each algorithm
        for (int i = 0; i < 4; ++i)
        {
            wxBitmap bitmap(32, 32);
            wxMemoryDC dc(bitmap);
            
            // Set different background colors for each algorithm
            wxColour colors[] = {wxColour(100, 200, 100), wxColour(100, 100, 200), 
                                wxColour(200, 200, 100), wxColour(200, 100, 100)};
            
            dc.SetBackground(wxBrush(colors[i % 4]));
            dc.Clear();
            
            // Draw a simple icon
            dc.SetPen(wxPen(wxColour(0, 0, 0), 2));
            dc.DrawRectangle(5, 5, 22, 22);
            dc.DrawLine(5, 5, 27, 27);
            dc.DrawLine(27, 5, 5, 27);
            
            dc.SelectObject(wxNullBitmap);
            icons.push_back(bitmap);
        }
        
        return icons;
    }

    void diff_control::open_containing_folder(const std::string &path)
    {
        if (path.empty())
            return;
            
        std::filesystem::path fs_path(path);
        if (!std::filesystem::exists(fs_path))
            return;
            
        std::string folder_path = fs_path.parent_path().string();
        
#ifdef _WIN32
        ShellExecuteA(nullptr, "explore", folder_path.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
#else
        std::string command = "xdg-open \"" + folder_path + "\"";
        std::system(command.c_str());
#endif
    }

    void diff_control::open_file_in_os(const std::string &path)
    {
        if (path.empty())
            return;
            
        std::filesystem::path fs_path(path);
        if (!std::filesystem::exists(fs_path))
            return;
            
#ifdef _WIN32
        ShellExecuteA(nullptr, "open", path.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
#else
        std::string command = "xdg-open \"" + path + "\"";
        std::system(command.c_str());
#endif
    }
} // namespace fons::gui