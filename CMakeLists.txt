Project(sequencer)
cmake_minimum_required(VERSION 3.11)

if(WIN32) # to mingw work as all the others
    set(CMAKE_SHARED_LIBRARY_PREFIX "")
endif(WIN32)

# General Settings
set(CMAKE_CXX_STANDARD 11)

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/externals/cimgui/imgui/backends)
    set(BAKENDS_FOLDER "externals/cimgui/imgui/backends/")
else()
    set(BAKENDS_FOLDER "externals/cimgui/imgui/examples/")
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/externals/cimgui/imgui/imgui_tables.cpp)
    set(TABLES_SOURCE "externals/cimgui/imgui/imgui_tables.cpp")
else()
    set(TABLES_SOURCE "")
endif()

# Include Directories
include_directories(externals/cimgui/imgui)
add_definitions("-DIMGUI_DISABLE_OBSOLETE_FUNCTIONS=1")

# SDL2 Settings
set(SDL2_DLL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/externals/SDL2-devel-2.28.4-VC/SDL2-2.28.4/lib/x64/SDL2.dll")

# # libuv Settings
# include(ExternalProject)

# set(LIBUV_SOURCE_DIR "${CMAKE_BINARY_DIR}/libuv-src")

# # don't download libuv if it already exists
# if(NOT EXISTS ${LIBUV_SOURCE_DIR})
# ExternalProject_Add(libuv
# GIT_REPOSITORY https://github.com/libuv/libuv.git
# GIT_TAG v1.x
# SOURCE_DIR ${LIBUV_SOURCE_DIR}
# BINARY_DIR "${CMAKE_BINARY_DIR}/libuv-build"
# CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/libuv-install
# INSTALL_COMMAND ""
# TEST_COMMAND ""
# )
# else()
# ExternalProject_Add(libuv
# DOWNLOAD_COMMAND ""
# SOURCE_DIR ${LIBUV_SOURCE_DIR}
# BINARY_DIR "${CMAKE_BINARY_DIR}/libuv-build"
# CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/libuv-install
# INSTALL_COMMAND ""
# TEST_COMMAND ""
# )
# endif()

# include_directories("${CMAKE_BINARY_DIR}/libuv-src/include")
# link_directories("${CMAKE_BINARY_DIR}/libuv-build")

# cimgui Settings
include_directories(externals/cimgui/)
set(CIMGUI_SRC
    externals/cimgui/cimgui.cpp
    externals/cimgui/imgui/imgui.cpp
    externals/cimgui/imgui/imgui_demo.cpp
    externals/cimgui/imgui/imgui_draw.cpp
    externals/cimgui/imgui/imgui_widgets.cpp
    ${TABLES_SOURCE}
    ${BAKENDS_FOLDER}/imgui_impl_sdl2.cpp
    ${BAKENDS_FOLDER}/imgui_impl_opengl3.cpp
)

set(IMGUI_LIBRARIES)

if(WIN32)
    add_definitions("-DIMGUI_IMPL_API=extern \"C\" __declspec\(dllexport\)")
else(WIN32)
    add_definitions("-DIMGUI_IMPL_API=extern \"C\" ")
endif(WIN32)

add_compile_definitions("IMGUI_IMPL_OPENGL_LOADER_GL3W")

# optional adding freetype
option(IMGUI_FREETYPE "add Freetype2" OFF)

if(IMGUI_FREETYPE)
    FIND_PACKAGE(freetype REQUIRED PATHS ${FREETYPE_PATH})
    list(APPEND IMGUI_LIBRARIES freetype)
    list(APPEND CIMGUI_SRC externals/cimgui/imgui/misc/freetype/imgui_freetype.cpp)
    add_definitions("-DCIMGUI_FREETYPE=1")
endif(IMGUI_FREETYPE)

# opengl3
list(APPEND CIMGUI_SRC ${BAKENDS_FOLDER}imgui_impl_opengl3.cpp)
include_directories(externals/cimgui/imgui/examples/libs/gl3w)

if(WIN32)
    list(APPEND IMGUI_LIBRARIES opengl32)
else(WIN32) # Unix
    if(APPLE)
        find_library(OPENGL_LIBRARY OpenGL)
        list(APPEND IMGUI_LIBRARIES ${OPENGL_LIBRARY})
    else()
        list(APPEND IMGUI_LIBRARIES GL)
    endif()
endif(WIN32)

set(SDL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/externals/SDL2-devel-2.28.4-VC/SDL2-2.28.4")

# sdl2
list(APPEND CIMGUI_SRC ${BAKENDS_FOLDER}imgui_impl_sdl2.cpp)

if(DEFINED SDL_PATH)
    message(STATUS "SDL_PATH defined as " ${SDL_PATH})
    FIND_PACKAGE(SDL2 PATHS ${SDL_PATH})
else(DEFINED SDL_PATH)
    # If SDL_PATH is not set, fallback and attempt to find SDL cmake script at a default location
    find_package(SDL2)
endif(DEFINED SDL_PATH)

if(SDL2_FOUND)
    get_target_property(SDL_INCLUDE SDL2::SDL2 INTERFACE_INCLUDE_DIRECTORIES)
    message(STATUS "sdlinclude is " ${SDL_INCLUDE})

    if("${SDL_INCLUDE}" STREQUAL "" OR "${SDL_INCLUDE}" STREQUAL "SDL_INCLUDE-NOTFOUND") # if not found latest SDL2 cmake config use older
        message(STATUS "sdlinclude2 is " ${SDL2_INCLUDE_DIRS})
        include_directories(${SDL2_INCLUDE_DIRS})
        set(IMGUI_SDL_LIBRARY ${SDL2_LIBRARIES})
        message(STATUS IMGUI_SDL_LIBRARY ${SDL2_LIBRARIES})
    else() # use new one SDL2 config
        include_directories(${SDL_INCLUDE})
        set(IMGUI_SDL_LIBRARY SDL2::SDL2)
        set(SDL_MAIN SDL2::SDL2main)
        message(STATUS ${SDL_MAIN} ${IMGUI_SDL_LIBRARY})
    endif()
else(SDL2_FOUND)
    if(DEFINED SDL_PATH)
        message(FATAL_ERROR "Cannot find SDL at SDL_PATH")
    else(DEFINED SDL_PATH)
        message(FATAL_ERROR "Cannot find SDL. Maybe try specifying SDL_PATH?")
    endif(DEFINED SDL_PATH)
endif(SDL2_FOUND)

add_library(cimgui_sdl SHARED ${CIMGUI_SRC})
target_link_libraries(cimgui_sdl ${IMGUI_LIBRARIES} ${IMGUI_SDL_LIBRARY})

include_directories(
    src/actions
    src/helper
    src/input
    src/io
    src/logger
    src/record
    src/recorder
    src/threads
    src/window
    deps/include
)

set(ACTIONS src/actions/actions.c)
set(HELPER src/helper/helper.c)
set(INPUT src/input/input.c)
set(IO src/io/io.c)
set(LOGGER src/logger/logger.c)
set(RECORD src/record/record.c)
set(RECORDER src/recorder/recorder.c)
set(THREADS src/threads/threads.c)
set(WINDOW src/window/window.c)

set(FILES deps/src/glad.c src/main.c ${ACTIONS} ${HELPER} ${INPUT} ${IO} ${LOGGER} ${RECORD} ${RECORDER} ${THREADS} ${WINDOW})

include_directories(deps/include)

include_directories(externals/cimgui/generator/output/)
add_executable(sequencer ${FILES})
target_compile_definitions(sequencer PUBLIC -DCIMGUI_USE_OPENGL3 -DCIMGUI_USE_SDL2)

if(WIN32)
    target_link_libraries(sequencer ${IMGUI_SDL_LIBRARY} cimgui_sdl Ws2_32 Iphlpapi OpenGL32)
else()
    target_link_libraries(sequencer ${IMGUI_SDL_LIBRARY} cimgui_sdl ${OPENGL_LIBRARY})
endif()

if(MINGW)
    target_link_options(sequencer PRIVATE "-mconsole")
endif()

# add_dependencies(sequencer libuv)

# Post Build Commands
add_custom_command(TARGET sequencer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${SDL2_DLL_PATH}"
    $<TARGET_FILE_DIR:sequencer>)

# if(WIN32)
# add_custom_command(TARGET sequencer POST_BUILD
# COMMAND ${CMAKE_COMMAND} -E copy_if_different
# "${CMAKE_BINARY_DIR}/libuv-build/Debug/uv.dll"
# $<TARGET_FILE_DIR:sequencer>)
# endif()
