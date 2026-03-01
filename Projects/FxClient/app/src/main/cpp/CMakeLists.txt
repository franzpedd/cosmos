# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html.
# For more examples on how to use CMake, see https://github.com/android/ndk-samples.

# Sets the minimum CMake version required for this project.
cmake_minimum_required(VERSION 3.22.1)

# Declares the project name. The project name can be accessed via ${ PROJECT_NAME},
# Since this is the top level CMakeLists.txt, the project name is also accessible
# with ${CMAKE_PROJECT_NAME} (both CMake variables are in-sync within the top level
# build script scope).
project("FXClient")

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.
#
# In this top level CMakeLists.txt, ${CMAKE_PROJECT_NAME} is used to define
# the target library name; in the sub-module's CMakeLists.txt, ${PROJECT_NAME}
# is preferred for the same purpose.
#
# In order to load a library into your app from Java/Kotlin, you must call
# System.loadLibrary() and pass the name of the library defined here;
# for GameActivity/NativeActivity derived applications, the same library name must be
# used in the AndroidManifest.xml file.
add_library(${CMAKE_PROJECT_NAME} SHARED
        # List C/C++ source files with relative paths to this CMakeLists.txt.
        native-lib.cpp
        ${PROJECT_NAME}/Application.h   ${PROJECT_NAME}/Application.cpp
        ${PROJECT_NAME}/CoreWidget.h    ${PROJECT_NAME}/CoreWidget.cpp
        ${PROJECT_NAME}/Bluetooth.h     ${PROJECT_NAME}/Bluetooth.cpp
        ${PROJECT_NAME}/Effect.h
)

# Import SDL3 (Shared Library)
set(SDL_DISABLE_PCH ON CACHE BOOL "Disable precompiled headers")
set(BUILD_AS_DLL ON CACHE BOOL "Build shared libraries" FORCE)
add_subdirectory(
        ../../../../../../Engine/Thirdparty/sdl
        ${CMAKE_CURRENT_BINARY_DIR}/sdl
)

# Import ImGui (Shared Library)
add_subdirectory(
        ../../../../../../Engine/Thirdparty/imgui
        ${CMAKE_CURRENT_BINARY_DIR}/imgui
)

# Import Engine (Shared Library)
add_subdirectory(
        ../../../../../../Engine
        ${CMAKE_CURRENT_BINARY_DIR}/Engine
)

# Specifies libraries CMake should link to your target library. You
# can link libraries from various origins, such as libraries defined in this
# build script, prebuilt third-party libraries, or Android system libraries.
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
        # List libraries link to the target library
        android
        log
        vulkan
        # List of necessary links
        PRIVATE SDL3::SDL3
        PRIVATE ImGui
        PRIVATE Engine
)

# Add libraries directories for searching code definition
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
        ../../../../../../Engine/Source
        ../../../../../../Engine/Thirdparty/evk/evk
        ../../../../../../Engine/Thirdparty/evk/evk/thirdparty
)
