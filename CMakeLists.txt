cmake_minimum_required(VERSION 3.10.2)
project(EvolutionaryRobotics_PredatorPreyModel VERSION 0.10)

if (WIN32) # Include win64 platforms
    find_package(OpenGL REQUIRED)
    find_library(GLEW NAMES glew32s PATHS external/lib/x64)
    find_library(GLFW NAMES glfw3 PATHS external/lib)

    set(INCLUDE_DIRS
        .
        agl
        agl/mesh
        external/include
        )

    set(LIBRARY_DIRS
        agl
        external/lib
        )

    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_FLAGS 
        "/wd4018 /wd4244 /wd4305 
        /D_CRT_SECURE_NO_WARNINGS 
        /D_CRT_NONSTDC_NO_DEPRECATE 
        /D NOMINMAX /DGLEW_STATIC
        /EHsc"
     )

    set(VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
    set(CMAKE_EXE_LINKER_FLAGS "/NODEFAULTLIB:\"MSVCRT\" /NODEFAULTLIB:\"LIBCMT\"")

    set(CORE_LIBS agl ${GLEW} ${GLFW} opengl32.lib)
    set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
    set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)

elseif (APPLE)

    set(CMAKE_MACOSX_RPATH 1)
    set(CMAKE_CXX_FLAGS "-Wall -Wno-deprecated-declarations -Wno-reorder-ctor -Wno-unused-function -Wno-unused-variable -g -stdlib=libc++ -std=c++14")
    find_library(GL_LIB OpenGL)
    find_library(GLFW glfw)
    add_definitions(-DAPPLE)
    set(CORE_LIBS agl ${GLFW} ${GL_LIB})

elseif (UNIX)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3 -march=native -g -std=c++20 -Wno-comment -Wno-sign-compare -Wno-reorder -Wno-unused-function -pthread")
    FIND_PACKAGE(OpenGL REQUIRED) 
    FIND_PACKAGE(GLEW REQUIRED)
    add_definitions(-DUNIX)
    set(CORE_LIBS GLEW glfw GL X11)

endif()
SET(CORE_LIBS "${CORE_LIBS}" CACHE INTERNAL "CORE_LIBS")

add_custom_target(RESOURCES_DIR SOURCES "${CMAKE_CURRENT_LIST_DIR}/res/")
add_custom_command(TARGET RESOURCES_DIR POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_LIST_DIR}/res/"
    "${CMAKE_CURRENT_LIST_DIR}/build/res/"
)   
# file(GLOB_RECURSE RESOURCES_DIR_FILES "${CMAKE_CURRENT_LIST_DIR}/res/*")
# add_custom_command(TARGET RESOURCES_DIR POST_BUILD COMMAND ${CMAKE_COMMAND} -E create_symlink
#                        "${CMAKE_CURRENT_LIST_DIR}/res/*" "${CMAKE_CURRENT_LIST_DIR}/build/res/*")

add_subdirectory("submodules/") # linking to submodules
add_subdirectory("src/") # adding sources
add_subdirectory("bin/") # adding libraries
