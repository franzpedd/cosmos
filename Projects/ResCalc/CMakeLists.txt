# ----------------------------------------------------------------------- configuration
cmake_minimum_required(VERSION 3.22.1)
project(ResCalc LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17) 
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)

# ----------------------------------------------------------------------- project
set(RESCALC_BASE_DIR ${CMAKE_SOURCE_DIR}/Projects/${PROJECT_NAME}/app/src/main/cpp)

file(GLOB_RECURSE SOURCES "${RESCALC_BASE_DIR}/ResCalc/*.h" "${RESCALC_BASE_DIR}/ResCalc/*.cpp" "${RESCALC_BASE_DIR}/native-lib.cpp")
add_executable(${PROJECT_NAME} ${SOURCES})
source_group(TREE ${RESCALC_BASE_DIR} PREFIX "Source Files" FILES ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC 
    ${RESCALC_BASE_DIR}/ResCalc
    ${CMAKE_SOURCE_DIR}/Engine/Source 
    ${CMAKE_SOURCE_DIR}/Engine/Thirdparty
    ${CMAKE_SOURCE_DIR}/Engine/Thirdparty/evk/evk 
    ${CMAKE_SOURCE_DIR}/Engine/Thirdparty/evk/evk/thirdparty
)

set_target_properties(${PROJECT_NAME} PROPERTIES  FOLDER "Projects" VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
target_link_libraries(${PROJECT_NAME} PRIVATE Engine ImGui)