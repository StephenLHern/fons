cmake_minimum_required (VERSION 3.15)

project ("Fons")

# Start Temporary wxWidgets Manual Library Link
# This will no longer be required when vcpkg wxWidgets library is patched to find these targets automatically

if(CMAKE_BUILD_TYPE MATCHES "64")
set(WINARCH x64)
else()
set(WINARCH x86)
endif()

set(libtype u)
unset(arch_suffix)
unset(path_prefix)
set(build_suffix -rel)

if(CMAKE_BUILD_TYPE MATCHES "^(Debug|RelWithDebInfo)$")
    set(libtype ud)
    set(path_prefix debug)
    set(build_suffix -dbg)

    add_definitions(-D_DEBUG)
endif()

set(build_suffix_rel -rel)
set(build_suffix_dbg -dbg)

if(VCPKG_TARGET_TRIPLET MATCHES -static)
    set(arch_suffix  -static)

    set(build_suffix_rel -static-rel)
    set(build_suffix_dbg -static-dbg)

    set(build_suffix -static${build_suffix})
else()
    add_definitions(-DWXUSINGDLL)
endif()

add_definitions(-D_UNICODE -DUNICODE -DwxUSE_GUI=1 -D__WXMSW__)

set(common_prefix    ${_VCPKG_INSTALLED_DIR}/${WINARCH}-windows${arch_suffix})
set(dbg_prefix       ${_VCPKG_INSTALLED_DIR}/${WINARCH}-windows${arch_suffix}/debug)
set(installed_prefix ${_VCPKG_INSTALLED_DIR}/${WINARCH}-windows${arch_suffix}/${path_prefix})

include_directories(${installed_prefix}/lib/msw${libtype})
include_directories(${common_prefix}/include)
set(wxWidgets_LIB_DIR ${installed_prefix}/lib)
set(wxWidgets_LIBRARIES
    ${wxWidgets_LIB_DIR}/wxbase31${libtype}_net.lib
    ${wxWidgets_LIB_DIR}/wxbase31${libtype}_xml.lib
    ${wxWidgets_LIB_DIR}/wxmsw31${libtype}_core.lib
    ${wxWidgets_LIB_DIR}/wxmsw31${libtype}_gl.lib
    ${wxWidgets_LIB_DIR}/wxmsw31${libtype}_xrc.lib
    ${wxWidgets_LIB_DIR}/wxmsw31${libtype}_html.lib
    ${wxWidgets_LIB_DIR}/wxmsw31${libtype}_stc.lib
    ${wxWidgets_LIB_DIR}/wxbase31${libtype}.lib
    winmm comctl32 oleacc rpcrt4 shlwapi version wsock32 opengl32
)

if(EXISTS ${wxWidgets_LIB_DIR}/wxregex${libtype}.lib)
    list(APPEND wxWidgets_LIBRARIES ${wxWidgets_LIB_DIR}/wxregex${libtype}.lib)
endif()

if(VCPKG_TARGET_TRIPLET MATCHES -static)
    unset(deb_suffix)
    if(CMAKE_BUILD_TYPE MATCHES "^(Debug|RelWithDebInfo)$")
        set(deb_suffix d)
    endif()

    foreach(lib_name libpng jpeg lzma tiff libexpat wxscintilla)
        file(
            GLOB lib_file
            ${wxWidgets_LIB_DIR}/${lib_name}*.lib
        )

        if(lib_file)
            list(APPEND wxWidgets_LIBRARIES ${lib_file})
        endif()
    endforeach()
endif()

set(WXRC ${common_prefix}/tools/wxwidgets/wxrc.exe)

set(ENV{PATH} "${dbg_prefix}/bin;${common_prefix}/bin;$ENV{PATH}")

# End Temporary wxWidgets Manual Library Link

set(wxWidgets_CONFIGURATION mswu)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(unofficial-wxwidgets REQUIRED)
find_package(GTest REQUIRED)
find_package(cereal REQUIRED)
find_path(LIBGIT2_INCLUDE_DIR git2.h REQUIRED)
find_library(LIBGIT2_LIBRARY git2 REQUIRED)

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

add_executable (Fons WIN32 "app_main.cpp" "GUI/frame_home.cpp" "GUI/frame_home.hpp"  "git/gitlib_manager.hpp" "common/command.hpp" "common/command.cpp" "app_main.hpp"  "git/commands/find_repos.hpp" "git/commands/find_repos.cpp"  "app_settings.cpp" "app_settings.hpp" "app_cmd_manager.hpp" "app_cmd_manager.cpp" "gui/sidebar_button.hpp" "gui/sidebar_button.cpp" "gui/contexts/context_repo.hpp" "gui/contexts/context_repo.cpp" "gui/repo_selector.cpp" "gui/search_completer.hpp" "gui/search_completer.cpp" "settings_observer.hpp" "git/commands/find_branches.cpp" "git/commands/find_branches.hpp" "gui/branch_selector.cpp" "gui/branch_selector.hpp" "git/git_mediator.cpp" "git/git_mediator.hpp" "git/git_observer.hpp" "git/commands/status.hpp" "git/commands/status.cpp"  "git/unique_libgit_ptr.hpp" "gui/contexts/context_branches.cpp" "gui/contexts/context_branches.hpp" "gui/contexts/context_commit.cpp" "gui/contexts/context_commit.hpp" "gui/contexts/context_merge.cpp" "gui/contexts/context_merge.hpp" "gui/contexts/context_changes.cpp" "gui/contexts/context_changes.hpp" "gui/contexts/context_remote.cpp" "gui/contexts/context_remote.hpp" "gui/contexts/context_settings.cpp" "gui/contexts/context_settings.hpp" "common/cmd_observer.hpp"  "common/optional_binary.hpp" "common/optional_json.hpp" "common/optional_nvp.hpp" "gui/everything_but_the_title_bar.cpp" "gui/everything_but_the_title_bar.hpp"    "git/commands/revwalk.hpp" "git/commands/revwalk.cpp" "common/cmd_event.hpp" "common/cmd_event.cpp" "gui/fons_wx.h" "common/cmd_instance.hpp" "git/commit.hpp" "gui/ebt_window_control_button.cpp" "gui/ebt_window_control_button.hpp" "gui/contexts/context_info.hpp" "gui/contexts/context_info.cpp" "app_icon.rc")

# Enables as many errors, validations as possible for the compiler being used
if (MSVC)
set(CMAKE_INCLUDE_SYSTEM_FLAG_CXX "/external:I ")
string(REGEX REPLACE "/W[3|4]" "/Wall" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	if( CMAKE_BUILD_TYPE STREQUAL "Debug")
	target_compile_options(Fons PRIVATE /analyze:external- /permissive- /fsanitize=address /Wall /WX /wd4626 /wd4625 /wd4866 /wd4820 /wd5026 /wd5027 /wd4514 /wd5045 /wd5039 /wd4868 /experimental:external /external:W0)
	else()
	target_compile_options(Fons PRIVATE)
	endif()
else()
target_compile_options(Fons PRIVATE -Wall -Wextra -Wpedantic -Werror -fsanitize=undefined,address)
endif()

target_include_directories(Fons SYSTEM PRIVATE ${LIBGIT2_INCLUDE_DIR})
target_include_directories(Fons SYSTEM PRIVATE ${wxWidgets_INCLUDE_DIRS})
target_include_directories(Fons SYSTEM PRIVATE ${cereal_INCLUDE_DIRS})

target_include_directories(Fons PRIVATE ".")

set_target_properties(Fons PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES $<TARGET_PROPERTY:dep,INTERFACE_INCLUDE_DIRECTORIES>)

target_link_libraries(Fons PRIVATE ${LIBGIT2_LIBRARY})
target_link_libraries(Fons PRIVATE wx::wxgl PRIVATE wx::wxqa PRIVATE wx::wxadv PRIVATE wx::wxaui)
#target_link_libraries(Fons PRIVATE GTest::gmock GTEST::gtest GTEST::gmock_main GTEST::gtest_main)
target_link_libraries(Fons PRIVATE ${wxWidgets_LIBRARIES})
target_link_libraries(Fons PRIVATE cereal)