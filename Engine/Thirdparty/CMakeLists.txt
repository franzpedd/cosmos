# ----------------------------------------------------------------------- general
find_package(Git REQUIRED)

# ----------------------------------------------------------------------- dependency: evk
set(EVK_DIR ${CMAKE_SOURCE_DIR}/Engine/Thirdparty/evk)
if(NOT EXISTS ${EVK_DIR})
    execute_process(COMMAND ${GIT_EXECUTABLE} clone https://github.com/franzpedd/evk.git ${EVK_DIR})
endif()

set(CMAKE_FOLDER "Thirdparty/EVK") 
    add_subdirectory(${EVK_DIR})
unset(CMAKE_FOLDER)

# ----------------------------------------------------------------------- dependency: sdl
set(SDL_DIR ${CMAKE_SOURCE_DIR}/Engine/Thirdparty/sdl)
if(NOT EXISTS ${SDL_DIR})
    execute_process(COMMAND ${GIT_EXECUTABLE} clone https://github.com/libsdl-org/SDL.git ${SDL_DIR})
endif()

set(CMAKE_FOLDER "Thirdparty/SDL") 
    add_subdirectory(${SDL_DIR})
unset(CMAKE_FOLDER)

# ----------------------------------------------------------------------- dependency: dear imgui
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/Engine/Thirdparty/imgui)
if(NOT EXISTS ${IMGUI_DIR})
    execute_process(COMMAND ${GIT_EXECUTABLE} clone --branch docking https://github.com/ocornut/imgui.git ${IMGUI_DIR})
endif()

set(IMGUI_CMAKELISTS ${IMGUI_DIR}/CMakeLists.txt)

# overwrite the imconfig.h file with desired config by cosmos framework
file(WRITE ${IMGUI_DIR}/imconfig.h "#pragma once

/// @brief DLL exporting for ImGui
#if defined(_WIN32) || defined(_WIN64)
    #if defined(COSMOS_IMGUI_SHARED)
        #if defined(COSMOS_IMGUI_EXPORT)
            #define IMGUI_API __declspec(dllexport)
        #else
            #define IMGUI_API __declspec(dllimport)
        #endif
    #else
        #define IMGUI_API
    #endif
#elif defined(__GNUC__) || defined(__clang__)
    #if defined(COSMOS_IMGUI_SHARED)
        #if defined(COSMOS_IMGUI_EXPORT)
            #define IMGUI_API __attribute__((visibility(\"default\")))
        #else
            #define IMGUI_API
        #endif
    #else
        #define IMGUI_API
    #endif
#else
    #error \"Unknown Compiler\"
#endif
")

file(WRITE ${IMGUI_CMAKELISTS} "
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY \${CMAKE_BINARY_DIR}/Bin)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY \${CMAKE_BINARY_DIR}/Bin)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY \${CMAKE_BINARY_DIR}/Bin)

    add_library(ImGui SHARED
        imconfig.h
        imgui_demo.cpp
        imgui_draw.cpp
        imgui_internal.h
        imgui_tables.cpp
        imgui_widgets.cpp
        imgui.cpp
        imgui.h
        imstb_rectpack.h
        imstb_textedit.h
        imstb_truetype.h
        backends/imgui_impl_sdl3.h
        backends/imgui_impl_vulkan.h
    )
    target_compile_definitions(ImGui PRIVATE COSMOS_IMGUI_EXPORT)
    target_compile_definitions(ImGui PRIVATE COSMOS_IMGUI_SHARED)
    target_include_directories(ImGui PUBLIC \${CMAKE_CURRENT_SOURCE_DIR})
")

set(CMAKE_FOLDER "Thirdparty/IMGUI")
    add_subdirectory(${IMGUI_DIR})
unset(CMAKE_FOLDER)