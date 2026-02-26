# ----------------------------------------------------------------------- configuration
cmake_minimum_required(VERSION 3.22.1)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMakeTools") 

# ----------------------------------------------------------------------- options
option(BUILD_PROJECTS "Build all projects and Cosmos" ON)

# ----------------------------------------------------------------------- projects
project(Solution VERSION 1.0 LANGUAGES CXX)

include(Engine/Thirdparty/CMakeLists.txt)
add_subdirectory(Engine)

if(BUILD_PROJECTS)
    add_subdirectory(Projects/Editor)
    add_subdirectory(Projects/ResCalc_Desktop)
endif()
