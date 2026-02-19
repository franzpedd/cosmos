# ----------------------------------------------------------------------- group files
function(add_group)
    foreach(file IN LISTS ARGN)
        get_filename_component(dir "${file}" DIRECTORY)

        if(dir STREQUAL "")
            set(group "root")
        else()
            string(REPLACE "/" "\\" group "${dir}")
        endif()

        source_group("${group}" FILES "${file}")
    endforeach()
endfunction()

# ----------------------------------------------------------------------- configuration
cmake_minimum_required(VERSION 3.22.1)
project(Engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17) 
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)

# ----------------------------------------------------------------------- project
set(SOURCES
    # Thirdparty: ImGui
    Thirdparty/imgui/imconfig.h
    Thirdparty/imgui/imgui_demo.cpp
    Thirdparty/imgui/imgui_draw.cpp
    Thirdparty/imgui/imgui_internal.h
    Thirdparty/imgui/imgui_tables.cpp
    Thirdparty/imgui/imgui_widgets.cpp
    Thirdparty/imgui/imgui.cpp
    Thirdparty/imgui/imgui.h
    Thirdparty/imgui/imstb_rectpack.h
    Thirdparty/imgui/imstb_textedit.h
    Thirdparty/imgui/imstb_truetype.h
    Thirdparty/imgui/backends/imgui_impl_sdl3.h
    Thirdparty/imgui/backends/imgui_impl_vulkan.h

    # Core
    Source/Engine.h
    Source/Core/Application.h       Source/Core/Application.cpp
    Source/Core/Defines.h
    Source/Core/Input.h
    Source/Core/Logger.h            Source/Core/Logger.cpp
    Source/Core/Renderer.h          Source/Core/Renderer.cpp
    Source/Core/Window.h            Source/Core/Window.cpp

    # UI
    Source/UI/GUI.h                 Source/UI/GUI.cpp
    Source/UI/Icons.h
    Source/UI/Images.h
    Source/UI/Widget.h              Source/UI/Widget.cpp

    # Util
    Source/Util/Clock.h
    Source/Util/Container.h
)
add_group(${SOURCES})
add_library(Engine SHARED ${SOURCES})

target_include_directories(Engine PUBLIC ${CMAKE_SOURCE_DIR}/Engine/Source ${CMAKE_SOURCE_DIR}/Engine/Thirdparty)
target_include_directories(Engine PUBLIC ${CMAKE_SOURCE_DIR}/Engine/Thirdparty/evk/evk ${CMAKE_SOURCE_DIR}/Engine/Thirdparty/evk/evk/thirdparty)

target_compile_definitions(Engine PRIVATE EVK_ENABLE_VALIDATIONS=1)
target_compile_definitions(Engine PRIVATE COSMOS_EXPORT)

set_target_properties(Engine PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:Engine>")
set_target_properties(Engine PROPERTIES FOLDER "Projects")

# ----------------------------------------------------------------------- extra

target_link_libraries(Engine PRIVATE SDL3::SDL3)

# copy the SDL3 into the build folder
add_custom_command(TARGET Engine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:SDL3::SDL3>"
        "$<TARGET_FILE_DIR:Engine>"
    VERBATIM
)
