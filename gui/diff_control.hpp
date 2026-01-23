
#pragma once

#include "git/commands/head_to_workdir.hpp"
#include "gui/fons_wx.hpp"
#include <vector>
#include <wx/stc/stc.h>
#include <wx/panel.h>
#include <wx/collpane.h>

namespace fons::gui
{
    enum class diff_algorithm
    {
        meyers,
        bentley_mcilroy,
        xdelta,
        bsdiff
    };

    struct diff_colors
    {
        wxColour added_lines = wxColour(200, 255, 200);
        wxColour removed_lines = wxColour(255, 200, 200);
        wxColour changed_lines = wxColour(255, 255, 200);
    };

    class diff_control : public wxPanel
    {
      public:
        diff_control(wxWindow *parent);
        diff_control(diff_control &other) = delete;
        diff_control &operator=(const diff_control &other) = delete;
        ~diff_control() override;

        void load_files(std::string_view left_path, std::string_view right_path);
        void load_content(std::string_view left_content, std::string_view right_content);

        void set_left_alias(std::string_view alias);
        void set_right_alias(std::string_view alias);

        void set_left_editable(bool editable);
        void set_right_editable(bool editable);

        void set_diff_algorithm(diff_algorithm algo);
        void set_diff_colors(const diff_colors &colors);

        void load_from_git_diff(const git::file_diff &diff);

      private:
        /**
         * @brief Panel containing all controls except file content
         */
        wxCollapsiblePane *controls_panel;
        
        /**
         * @brief Radio buttons for algorithm selection with icons
         */
        wxRadioButton *meyers_radio;
        wxRadioButton *bentley_mcilroy_radio;
        wxRadioButton *xdelta_radio;
        wxRadioButton *bsdiff_radio;
        
        /**
         * @brief Static bitmaps for algorithm icons (placeholder)
         */
        wxStaticBitmap *meyers_icon;
        wxStaticBitmap *bentley_mcilroy_icon;
        wxStaticBitmap *xdelta_icon;
        wxStaticBitmap *bsdiff_icon;
        
        /**
         * @brief File path display and action buttons
         */
        wxStaticText *left_path_text;
        wxStaticText *right_path_text;
        wxBitmapButton *left_folder_button;
        wxBitmapButton *right_folder_button;
        wxBitmapButton *left_file_button;
        wxBitmapButton *right_file_button;
        
        /**
         * @brief Text controls for file content
         */
        wxStyledTextCtrl *left_text;
        wxStyledTextCtrl *right_text;
        
        /**
         * @brief Scrollable panel containing file contents
         */
        wxScrolledWindow *content_scroll_window;

        std::string left_alias;
        std::string right_alias;
        std::string left_path;
        std::string right_path;

        diff_algorithm current_algorithm;
        diff_colors current_colors;

        bool left_editable;
        bool right_editable;

        /**
         * @brief Create the user interface
         */
        void create_ui();
        
        /**
         * @brief Create placeholder icons for algorithms
         * @return Vector of created bitmaps
         */
        std::vector<wxBitmap> create_algorithm_icons();
        
        /**
         * @brief Update the diff display
         */
        void update_diff();
        
        /**
         * @brief Split content into lines
         * @param content Text content to split
         * @return Vector of lines
         */
        std::vector<std::string> split_lines(const std::string &content);
        
        /**
         * @brief Apply diff highlighting to text control
         * @param lines Lines to apply
         * @param control Text control to modify
         * @param line_statuses Status for each line (-1: removed, 0: unchanged, 1: added, 2: changed)
         */
        void apply_diff_lines(const std::vector<std::string> &lines, wxStyledTextCtrl *control, const std::vector<int> &line_statuses);
        
        /**
         * @brief Compute line differences between two files
         * @param left_lines Left file lines
         * @param right_lines Right file lines
         * @param left_statuses Output status for left lines
         * @param right_statuses Output status for right lines
         * @return Length of longest common subsequence
         */
        size_t compute_line_diff(const std::vector<std::string> &left_lines, const std::vector<std::string> &right_lines,
                                 std::vector<int> &left_statuses, std::vector<int> &right_statuses);
        
        /**
         * @brief Calculate length of longest common subsequence
         * @param a First string
         * @param b Second string
         * @return LCS length
         */
        size_t lcs_length(const std::string &a, const std::string &b);
        
        /**
         * @brief Compute longest common subsequence
         * @param left_lines Left file lines
         * @param right_lines Right file lines
         * @param lcs Output LCS pairs
         * @return LCS length
         */
        size_t compute_lcs(const std::vector<std::string> &left_lines, const std::vector<std::string> &right_lines,
                           std::vector<std::pair<size_t, size_t>> &lcs);
        
        /**
         * @brief Open containing folder for given path
         * @param path File path
         */
        void open_containing_folder(const std::string &path);
        
        /**
         * @brief Open file in default OS application
         * @param path File path
         */
        void open_file_in_os(const std::string &path);
    };
} // namespace fons::gui