############################################################################################################################################
# Fons Unit Test Cmake
############################################################################################################################################

cmake_minimum_required (VERSION 3.8)

enable_testing()

project ("Fons-UT")

set(wxWidgets_CONFIGURATION mswu)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

############################################################################################################################################
# Find Required Build Dependencies
############################################################################################################################################

find_package(GTest CONFIG REQUIRED)
find_package(wxWidgets REQUIRED COMPONENTS core base stc OPTIONAL_COMPONENTS net)

############################################################################################################################################
# Establish Unit Test Executable
############################################################################################################################################

add_executable (Fons-UT 
"app_cmd_manager_tests.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../app_cmd_manager.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/../common/command.cpp"
 "ut_main.cpp")

############################################################################################################################################
# Include Directories
############################################################################################################################################

target_include_directories(Fons-UT PRIVATE
"${CMAKE_CURRENT_SOURCE_DIR}/../"
"${CMAKE_CURRENT_SOURCE_DIR}/../common/")
target_include_directories(Fons-UT SYSTEM PRIVATE ${wxWidgets_INCLUDE_DIRS})

############################################################################################################################################
# Compilation Settings
############################################################################################################################################

target_compile_definitions(Fons-UT PRIVATE ${wxWidgets_DEFINITIONS} "$<$<CONFIG:DEBUG>:${wxWidgets_DEFINITIONS_DEBUG}>")

############################################################################################################################################
# Linked Libraries
############################################################################################################################################

target_link_libraries(Fons-UT PRIVATE GTest::gtest GTest::gmock)
target_link_libraries(Fons-UT PRIVATE ${wxWidgets_LIBRARIES})

############################################################################################################################################
# Enable CMake test runner to discover google tests
############################################################################################################################################

include(GoogleTest)
gtest_discover_tests(Fons-UT)