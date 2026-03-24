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
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Build)

# ----------------------------------------------------------------------- project
set(SOURCES

    # Core
    Source/Engine.h
    Source/Core/Application.h       Source/Core/Application.cpp
    Source/Core/Defines.h
    Source/Core/Input.h
    Source/Core/Logger.h            Source/Core/Logger.cpp
    Source/Core/Renderer.h          Source/Core/Renderer.cpp
    Source/Core/Window.h            Source/Core/Window.cpp

    # Scene
    Source/Scene/Components.h       Source/Scene/Components.cpp
    Source/Scene/Entity.h
    Source/Scene/World.h            Source/Scene/World.cpp

    # UI
    Source/UI/GUI.h                 Source/UI/GUI.cpp
    Source/UI/Icons.h
    Source/UI/Images.h
    Source/UI/Widget.h              Source/UI/Widget.cpp

    # Util
    Source/Util/Clock.h
    Source/Util/Container.h
    Source/Util/Datafile.h
    Source/Util/ID.h
    Source/Util/Library.h
)
add_group(${SOURCES})
add_library(Engine SHARED ${SOURCES})

target_include_directories(Engine PUBLIC ${CMAKE_SOURCE_DIR}/Engine/Source ${CMAKE_SOURCE_DIR}/Thirdparty)
target_include_directories(Engine PUBLIC ${CMAKE_SOURCE_DIR}/Thirdparty/evk/EVK ${CMAKE_SOURCE_DIR}/Thirdparty/evk/EVK/Thirdparty)

target_compile_definitions(Engine PRIVATE VECMATH_BUILD_SHARED VECMATH_EXPORT)
target_compile_definitions(Engine PRIVATE CTOOLBOX_BUILD_SHARED CTOOLBOX_EXPORT)
target_compile_definitions(Engine PRIVATE MEMM_BUILD_SHARED MEMM_EXPORT)
target_compile_definitions(Engine PRIVATE EVK_BUILD_DYNAMIC EVK_EXPORT)
target_compile_definitions(Engine PRIVATE EVK_ENABLE_VALIDATIONS)
target_compile_definitions(Engine PRIVATE COSMOS_EXPORT)

set_target_properties(Engine PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:Engine>")
set_target_properties(Engine PROPERTIES FOLDER "Engine")

target_link_libraries(Engine PRIVATE SDL3::SDL3 ImGui)

# ----------------------------------------------------------------------- extra

# copy assets to build directory
if(WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Assets"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/Assets"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Assets"
        COMMENT "Copying assets to build directory"
        VERBATIM
    )
else()
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/Assets"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Assets"
        COMMENT "Copying assets to build directory"
        VERBATIM
    )
endif()

# add android stuff on build process
if(ANDROID)
    target_link_libraries(Engine PRIVATE android log)
endif()

# copy the SDL3 into the build folder
add_custom_command(TARGET Engine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:SDL3::SDL3>"
        "$<TARGET_FILE_DIR:Engine>"
    VERBATIM
)