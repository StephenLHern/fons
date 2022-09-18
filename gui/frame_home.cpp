// MSWWindowProc based on code suggested by Reza, Orkhan Aliyev
// https://stackoverflow.com/questions/41179437/wxwidgets-wxborder-none-and-wxresize-border-makes-white-area

#include "frame_home.hpp"
#include "app_main.hpp"
#include "contexts/context_branches.hpp"
#include "contexts/context_changes.hpp"
#include "contexts/context_commit.hpp"
#include "contexts/context_info.hpp"
#include "contexts/context_merge.hpp"
#include "contexts/context_remote.hpp"
#include "contexts/context_repo.hpp"
#include "contexts/context_settings.hpp"
#include "everything_but_the_title_bar.hpp"
#include "gui/ebt_window_control_button.hpp"
#include "sidebar_button.hpp"

#include <windowsx.h> // Must be included after WxWidgets headers, needed for GET_X_LPARAM and GET_Y_LPARAM macros

namespace fons::gui
{
    frame_home::frame_home(wxApp *parent)
        : wxFrame(NULL, wxID_ANY, "Fons", wxDefaultPosition, wxDefaultSize, ((wxSYSTEM_MENU | wxRESIZE_BORDER | wxCLIP_CHILDREN)))
    {
        parent_app = dynamic_cast<app_main *>(parent);

        Bind(wxEVT_BUTTON, &frame_home::on_sidebar_select, this);
        Bind(wxEVT_CLOSE_WINDOW, &app_main::on_close, parent_app);

        wxInitAllImageHandlers();

        SetIcon(wxICON(IDI_APPICON));

        auto *main_panel = new wxPanel(this, -1);
        main_panel->SetBackgroundColour(wxColour(243, 243, 243));

        auto *repo_button = new sidebar_button(main_panel, -1, "Repository", "images/repo-24.png");
        repo_button->mark_active();
        auto *branch_button = new sidebar_button(main_panel, -1, "Branches", "images/git-branch-24.png");
        auto *changes_button = new sidebar_button(main_panel, -1, "Changes", "images/pencil-24.png");
        auto *commit_button = new sidebar_button(main_panel, -1, "Commit", "images/git-commit-24.png");
        auto *merge_button = new sidebar_button(main_panel, -1, "Merge", "images/git-pull-request-24.png");
        auto *remote_button = new sidebar_button(main_panel, -1, "Remote", "images/workflow-24.png");
        auto *settings_button = new sidebar_button(main_panel, -1, "Settings", "images/gear-24.png");
        auto *info_button = new sidebar_button(main_panel, -1, "Info", "images/info-24.png");

        context_book = new wxSimplebook(main_panel, -1);
        context_book->AddPage(new context_repo(context_book, parent_app->settings, parent_app->git_mediator.value()), "Repository");
        context_book->AddPage(new context_branches(context_book, parent_app->settings, parent_app->git_mediator.value()), "Branches");
        context_book->AddPage(
            new context_changes(context_book, parent_app->settings, parent_app->git_mediator.value(), parent_app->cmd_manager), "Changes");
        context_book->AddPage(new context_commit(context_book), "Commit");
        context_book->AddPage(new context_merge(context_book, parent_app->settings, parent_app->git_mediator.value()), "Merge");
        context_book->AddPage(new context_remote(context_book, parent_app->settings, parent_app->git_mediator.value()), "Remote");
        context_book->AddPage(
            new context_settings(context_book, parent_app->settings, parent_app->git_mediator.value(), parent_app->cmd_manager),
            "Settings");
        context_book->AddPage(new context_info(context_book), "Info");

        button_id_to_page_id.insert(std::pair<wxWindowID, size_t>(repo_button->GetId(), 0));
        button_id_to_page_id.insert(std::pair<wxWindowID, size_t>(branch_button->GetId(), 1));
        button_id_to_page_id.insert(std::pair<wxWindowID, size_t>(changes_button->GetId(), 2));
        button_id_to_page_id.insert(std::pair<wxWindowID, size_t>(commit_button->GetId(), 3));
        button_id_to_page_id.insert(std::pair<wxWindowID, size_t>(merge_button->GetId(), 4));
        button_id_to_page_id.insert(std::pair<wxWindowID, size_t>(remote_button->GetId(), 5));
        button_id_to_page_id.insert(std::pair<wxWindowID, size_t>(settings_button->GetId(), 6));
        button_id_to_page_id.insert(std::pair<wxWindowID, size_t>(info_button->GetId(), 7));

        auto *everything_but_the_title_bar = new fons::gui::everything_but_the_title_bar(main_panel, parent);

        auto *navigation_sizer = new wxBoxSizer(wxOrientation::wxHORIZONTAL);
        navigation_sizer->Add(everything_but_the_title_bar, wxSizerFlags(1).Expand());

        auto *sidebar_sizer = new wxBoxSizer(wxOrientation::wxVERTICAL);
        sidebar_sizer->Add(repo_button, wxSizerFlags().CenterHorizontal().FixedMinSize());
        sidebar_sizer->Add(branch_button, wxSizerFlags().CenterHorizontal().FixedMinSize());
        sidebar_sizer->Add(changes_button, wxSizerFlags().CenterHorizontal().FixedMinSize());
        sidebar_sizer->Add(commit_button, wxSizerFlags().CenterHorizontal().FixedMinSize());
        sidebar_sizer->Add(merge_button, wxSizerFlags().CenterHorizontal().FixedMinSize());
        sidebar_sizer->Add(remote_button, wxSizerFlags().CenterHorizontal().FixedMinSize());
        sidebar_sizer->Add(settings_button, wxSizerFlags().CenterHorizontal().FixedMinSize());
        sidebar_sizer->Add(info_button, wxSizerFlags().CenterHorizontal().FixedMinSize());

        auto *context_sizer = new wxBoxSizer(wxOrientation::wxHORIZONTAL);
        context_sizer->Add(sidebar_sizer, wxSizerFlags().Left().FixedMinSize());
        context_sizer->Add(context_book, wxSizerFlags(1).Expand());

        auto *workspace_sizer = new wxBoxSizer(wxOrientation::wxVERTICAL);
        workspace_sizer->Add(navigation_sizer, 0, wxEXPAND);
        workspace_sizer->Add(context_sizer, wxSizerFlags(1).Expand());

        main_panel->SetBackgroundColour(wxColour(243, 243, 243));
        main_panel->SetSizerAndFit(workspace_sizer);

        auto *frameSizer = new wxBoxSizer(wxOrientation::wxHORIZONTAL);
        frameSizer->Add(main_panel, wxSizerFlags(1).Expand());
        SetSizerAndFit(frameSizer);
    }

    void frame_home::on_user_code_generate(wxCommandEvent &event_data)
    {
        wxMessageBox(wxT("Visit https://github.com/login/device \r\n Enter User Code: " + event_data.GetString()), wxT("Device Activation"),
                     wxICON_INFORMATION);
    }

    void frame_home::on_sidebar_select(wxCommandEvent &event_data)
    {
        if (!button_id_to_page_id.contains(event_data.GetId()))
            return;

        context_book->SetSelection(button_id_to_page_id[event_data.GetId()]);

        for (std::pair<wxWindowID, size_t> element : button_id_to_page_id)
        {
            sidebar_button *button = static_cast<sidebar_button *>(FindWindowById(element.first));
            if (event_data.GetId() == button->GetId())
            {
                button->mark_active();
            }
            else
            {
                button->mark_inactive();
            }
        }
    }

    WXLRESULT frame_home::MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
    {
        switch (nMsg)
        {
        case WM_NCACTIVATE:
        {
            lParam = -1;
            break;
        }
        case WM_NCCALCSIZE:
        {
            if (wParam)
            {
                WINDOWPLACEMENT wPos;
                wPos.length = sizeof(wPos);
                GetWindowPlacement(GetHandle(), &wPos);
                if (wPos.showCmd != SW_SHOWMAXIMIZED)
                {
                    RECT borderThickness;
                    SetRectEmpty(&borderThickness);
                    AdjustWindowRectEx(&borderThickness, static_cast<DWORD>(GetWindowLongPtr(GetHandle(), GWL_STYLE) & ~WS_CAPTION), FALSE,
                                       NULL);
                    borderThickness.left *= -1;
                    borderThickness.top *= -1;
                    NCCALCSIZE_PARAMS *sz = reinterpret_cast<NCCALCSIZE_PARAMS *>(lParam);
                    sz->rgrc[0].top += 1;
                    sz->rgrc[0].left += borderThickness.left;
                    sz->rgrc[0].right -= borderThickness.right;
                    sz->rgrc[0].bottom -= borderThickness.bottom;
                    return 0;
                }
            }
            break;
        }
        case WM_NCHITTEST:
        {
            ebt_window_control_button *maximize = (ebt_window_control_button *)wxWindow::FindWindowById(wxID_MAXIMIZE_FRAME);
            if (maximize && maximize->selected)
                return HTMAXBUTTON;

            ebt_window_control_button *close = (ebt_window_control_button *)wxWindow::FindWindowById(wxID_CLOSE_FRAME);
            if (close && close->selected)
                return HTCLOSE;

            ebt_window_control_button *minimize = (ebt_window_control_button *)wxWindow::FindWindowById(wxID_ICONIZE_FRAME);
            if (minimize && minimize->selected)
                return HTMINBUTTON;

            break;
        }
        }
        return wxFrame::MSWWindowProc(nMsg, wParam, lParam);
    }
} // namespace fons::gui