############################################################################################################################################
# Fons Application Cmake
############################################################################################################################################

cmake_minimum_required (VERSION 3.15)

project ("Fons")

set(wxWidgets_CONFIGURATION mswu)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Load custom cmake modules
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

############################################################################################################################################
# Find Required Build Dependencies
############################################################################################################################################

find_package(wxWidgets REQUIRED COMPONENTS core base stc OPTIONAL_COMPONENTS net)
find_package(cereal REQUIRED)
find_path(LIBGIT2_INCLUDE_DIR git2.h REQUIRED)
find_library(LIBGIT2_LIBRARY git2 REQUIRED)

############################################################################################################################################
# Copied Files Required at Fons Runtime
############################################################################################################################################

# License Info
configure_file("gui/license_notices.txt" "license_notices.txt" COPYONLY)

# Sidebar Buttons (Inactive)
configure_file("gui/images/repo-24.png" "images/repo-24.png" COPYONLY)
configure_file("gui/images/git-branch-24.png" "images/git-branch-24.png" COPYONLY)
configure_file("gui/images/pencil-24.png" "images/pencil-24.png" COPYONLY)
configure_file("gui/images/git-commit-24.png" "images/git-commit-24.png" COPYONLY)
configure_file("gui/images/git-pull-request-24.png" "images/git-pull-request-24.png" COPYONLY)
configure_file("gui/images/workflow-24.png" "images/workflow-24.png" COPYONLY)
configure_file("gui/images/gear-24.png" "images/gear-24.png" COPYONLY)
configure_file("gui/images/info-24.png" "images/info-24.png" COPYONLY)

# Sidebar Buttons (Active)
configure_file("gui/images/repo-24-active.png" "images/repo-24-active.png" COPYONLY)
configure_file("gui/images/git-branch-24-active.png" "images/git-branch-24-active.png" COPYONLY)
configure_file("gui/images/pencil-24-active.png" "images/pencil-24-active.png" COPYONLY)
configure_file("gui/images/git-commit-24-active.png" "images/git-commit-24-active.png" COPYONLY)
configure_file("gui/images/git-pull-request-24-active.png" "images/git-pull-request-24-active.png" COPYONLY)
configure_file("gui/images/workflow-24-active.png" "images/workflow-24-active.png" COPYONLY)
configure_file("gui/images/gear-24-active.png" "images/gear-24-active.png" COPYONLY)
configure_file("gui/images/info-24-active.png" "images/info-24-active.png" COPYONLY)

# Everything But The Title Bar Window Control Buttons
configure_file("gui/images/win-close-active.png" "images/win-close-active.png" COPYONLY)
configure_file("gui/images/win-close-inactive.png" "images/win-close-inactive.png" COPYONLY)
configure_file("gui/images/win-close-selected.png" "images/win-close-selected.png" COPYONLY)
configure_file("gui/images/win-maximize-active.png" "images/win-maximize-active.png" COPYONLY)
configure_file("gui/images/win-maximize-inactive.png" "images/win-maximize-inactive.png" COPYONLY)
configure_file("gui/images/win-maximize-active.png" "images/win-maximize-selected.png" COPYONLY)
configure_file("gui/images/win-minimize-active.png" "images/win-minimize-active.png" COPYONLY)
configure_file("gui/images/win-minimize-inactive.png" "images/win-minimize-inactive.png" COPYONLY)
configure_file("gui/images/win-minimize-active.png" "images/win-minimize-selected.png" COPYONLY)
configure_file("gui/images/win-restore-active.png" "images/win-restore-active.png" COPYONLY)
configure_file("gui/images/win-restore-inactive.png" "images/win-restore-inactive.png" COPYONLY)
configure_file("gui/images/win-restore-active.png" "images/win-restore-selected.png" COPYONLY)

############################################################################################################################################
# Establish Fons Executable
############################################################################################################################################

add_executable (Fons WIN32 
"app_cmd_manager.cpp"
"app_icon.rc"
"app_main.cpp"
"app_settings.cpp"
"common/command.cpp"
"git/commands/find_branches.cpp"
"git/commands/find_repos.cpp"
"git/commands/revwalk.cpp"
"git/commands/status.cpp"
"git/git_mediator.cpp"
"gui/branch_selector.cpp"
"gui/contexts/context_branches.cpp"
"gui/contexts/context_changes.cpp"
"gui/contexts/context_commit.cpp"
"gui/contexts/context_info.cpp"
"gui/contexts/context_merge.cpp"
"gui/contexts/context_remote.cpp"
"gui/contexts/context_repo.cpp"
"gui/contexts/context_settings.cpp"
"gui/ebt_window_control_button.cpp"
"gui/everything_but_the_title_bar.cpp"
"gui/frame_home.cpp"
"gui/repo_selector.cpp"
"gui/search_completer.cpp"
"gui/sidebar_button.cpp"
"git/commands/head_to_workdir.cpp"
"git/commands/find_remotes.cpp"
"git/commands/get_config.cpp")

############################################################################################################################################
# Doxygen
############################################################################################################################################

add_subdirectory ("docs")

############################################################################################################################################
# Unit Tests
############################################################################################################################################

add_subdirectory ("ut")

############################################################################################################################################
# Include Directories
############################################################################################################################################

set_target_properties(Fons PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES $<TARGET_PROPERTY:dep,INTERFACE_INCLUDE_DIRECTORIES>)
target_include_directories(Fons SYSTEM PRIVATE ${LIBGIT2_INCLUDE_DIR})
target_include_directories(Fons SYSTEM PRIVATE ${wxWidgets_INCLUDE_DIRS})
target_include_directories(Fons SYSTEM PRIVATE ${cereal_INCLUDE_DIRS})
target_include_directories(Fons PRIVATE ".")

############################################################################################################################################
# Compilation Settings
############################################################################################################################################

# Enables as many errors, validations as possible for the compiler being used
if (MSVC)
set(CMAKE_INCLUDE_SYSTEM_FLAG_CXX "/external:I ")
string(REGEX REPLACE "/W[3|4]" "/Wall" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	if( CMAKE_BUILD_TYPE STREQUAL "Debug")
	target_compile_options(Fons PRIVATE /analyze:external- /permissive- /fsanitize=address /Wall /WX /wd4626 /wd4625 /wd4866 /wd4820 /wd5026
        /wd5027 /wd4514 /wd5045 /wd5039 /wd4868 /experimental:external /external:W0)
	else()
	target_compile_options(Fons PRIVATE)
	endif()
else()
target_compile_options(Fons PRIVATE -Wall -Wextra -Wpedantic -Werror -fsanitize=undefined,address)
endif()

target_compile_definitions(Fons PRIVATE ${wxWidgets_DEFINITIONS} "$<$<CONFIG:DEBUG>:${wxWidgets_DEFINITIONS_DEBUG}>")

############################################################################################################################################
# Linked Libraries
############################################################################################################################################

target_link_libraries(Fons PRIVATE ${LIBGIT2_LIBRARY})
target_link_libraries(Fons PRIVATE ${wxWidgets_LIBRARIES})

############################################################################################################################################
# WxWidgets Resource Compiler
############################################################################################################################################

option(USE_WXRC "Use the wxrc resource compiler" ON)
if(USE_WXRC)
    execute_process(
        COMMAND "${wxWidgets_wxrc_EXECUTABLE}" --help
        RESULTS_VARIABLE error_result
    )
    if(error_result)
        message(FATAL_ERROR "Failed to run wxWidgets_wxrc_EXECUTABLE (${wxWidgets_wxrc_EXECUTABLE})")
    endif()
endif()