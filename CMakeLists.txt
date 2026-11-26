cmake_minimum_required(VERSION 3.16)
project(NomadCommandCPP LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

if(ANDROID)
    add_library(native_app_glue STATIC ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
    include_directories(${ANDROID_NDK}/sources/android/native_app_glue)
    add_library(nomad_gl SHARED
        src/android_main.cpp
        src/ObjLoader.cpp
        src/RendererGL.cpp
        src/Engine.cpp
    )
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")
    target_link_libraries(nomad_gl native_app_glue android log EGL GLESv3)
else()
    find_package(SDL2 REQUIRED)
    include_directories(${SDL2_INCLUDE_DIRS})
    add_executable(nomad_gl
        src/main.cpp
        src/ObjLoader.cpp
        src/RendererGL.cpp
        src/Engine.cpp
    )
    target_link_libraries(nomad_gl ${SDL2_LIBRARIES} GL)
endif()
