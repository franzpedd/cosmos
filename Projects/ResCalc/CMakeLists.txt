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
project(ResCalc LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17) 
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Build)

# ----------------------------------------------------------------------- project
set(SOURCES
    # Core
    Source/Application.h          Source/Application.cpp
    Source/ResCalcWidget.h        Source/ResCalcWidget.cpp

    # Entrypoint
    Source/main.cpp
)
add_group(${SOURCES})
add_executable(ResCalc ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC 
    ${FXCLIENT_BASE_DIR}
    ${CMAKE_SOURCE_DIR}/Engine/Source 
    ${CMAKE_SOURCE_DIR}/Thirdparty
    ${CMAKE_SOURCE_DIR}/Thirdparty/evk/EVK 
    ${CMAKE_SOURCE_DIR}/Thirdparty/evk/EVK/Thirdparty
)

set_target_properties(${PROJECT_NAME} PROPERTIES  FOLDER "Projects" VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
target_link_libraries(${PROJECT_NAME} PRIVATE Engine ImGui)