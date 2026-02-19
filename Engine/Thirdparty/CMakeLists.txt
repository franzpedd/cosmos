# ----------------------------------------------------------------------- general
find_package(Git REQUIRED)

# ----------------------------------------------------------------------- dependency: evk
set(EVK_DIR ${CMAKE_SOURCE_DIR}/Engine/Thirdparty/evk)
if(NOT EXISTS ${EVK_DIR})
    execute_process(COMMAND ${GIT_EXECUTABLE} clone https://github.com/franzpedd/evk.git ${EVK_DIR})
endif()

set(CMAKE_FOLDER "Thirdparty/EVK") 
    add_subdirectory(${CMAKE_SOURCE_DIR}/Engine/Thirdparty/evk)
unset(CMAKE_FOLDER)

# ----------------------------------------------------------------------- dependency: sdl
set(SDL_DIR ${CMAKE_SOURCE_DIR}/Engine/Thirdparty/sdl)
if(NOT EXISTS ${SDL_DIR})
    execute_process(COMMAND ${GIT_EXECUTABLE} clone https://github.com/libsdl-org/SDL.git ${SDL_DIR})
endif()

set(CMAKE_FOLDER "Thirdparty/SDL") 
    add_subdirectory(${CMAKE_SOURCE_DIR}/Engine/Thirdparty/sdl)
unset(CMAKE_FOLDER)

# ----------------------------------------------------------------------- dependency: dear imgui
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/Engine/Thirdparty/imgui)
if(NOT EXISTS ${IMGUI_DIR})
    execute_process(COMMAND ${GIT_EXECUTABLE} clone --branch docking https://github.com/ocornut/imgui.git  ${IMGUI_DIR})
endif()