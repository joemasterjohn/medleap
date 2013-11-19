cmake_minimum_required(VERSION 2.8)
project(medleap)


# output the executable in the build directory (not Debug/Release subdir)
# -----------------------------------------------------------------------------------------------------------------------
foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_CURRENT_BINARY_DIR})
endforeach(OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)
# -----------------------------------------------------------------------------------------------------------------------


# make sure resources are copied into the build directory
# -----------------------------------------------------------------------------------------------------------------------
set(DATA_SHADERS
    shaders/color.vert
    shaders/color.frag
    shaders/slice2D.vert
    shaders/slice2D.frag
)
set(DATA_FONTS
    fonts/menlo14.bmp
    fonts/menlo14.dat
    fonts/menlo18.bmp
    fonts/menlo18.dat
    fonts/menlo24.bmp
    fonts/menlo24.dat
)
foreach(item IN LISTS DATA_SHADERS DATA_FONTS)
    add_custom_command(
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${item}" 
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${item}" "${CMAKE_CURRENT_BINARY_DIR}/${item}"
        DEPENDS   "${CMAKE_CURRENT_SOURCE_DIR}/${item}"
    ) 
endforeach()
add_custom_target(SHADER_COPY ALL DEPENDS ${DATA_SHADERS} ${DATA_FONTS})
# -----------------------------------------------------------------------------------------------------------------------


# source group naming for IDEs
# -----------------------------------------------------------------------------------------------------------------------
source_group("" FILES src/main.cpp)
file(GLOB SOURCE_GL src/gl/*.cpp src/gl/*.h)
source_group("gl" FILES ${SOURCE_GL})
file(GLOB SOURCE_VOLUME src/volume/*.cpp src/volume/*.h)
source_group("volume" FILES ${SOURCE_VOLUME})
source_group("shaders" FILES ${DATA_SHADERS})
file(GLOB SOURCE_RENDER src/render/*.cpp src/render/*.h)
source_group("render" FILES ${SOURCE_RENDER})
file(GLOB SOURCE_MATH src/math/*.cpp src/math/*.h)
source_group("math" FILES ${SOURCE_MATH})
# -----------------------------------------------------------------------------------------------------------------------


# main executable
# -----------------------------------------------------------------------------------------------------------------------
add_executable(${PROJECT_NAME}
    src/main.cpp
    ${SOURCE_GL}
    ${SOURCE_VOLUME}
    ${SOURCE_MATH}
    ${SOURCE_RENDER}
    ${DATA_SHADERS}
)
add_dependencies(${PROJECT_NAME} SHADER_COPY)
# -----------------------------------------------------------------------------------------------------------------------


# libraries
# -----------------------------------------------------------------------------------------------------------------------
include_directories(src)
add_subdirectory(deps/glew)
include_directories(deps/glew/include)
find_package(GDCM REQUIRED)
include_directories(${GDCM_INCLUDE_DIRS})
find_package(GLFW REQUIRED)
include_directories(${GLFW_INCLUDE_DIR})

if (APPLE)
    find_library(OPENGL_LIBRARY OpenGL)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COREVIDEO_LIBRARY CoreVideo)
    target_link_libraries(
        ${PROJECT_NAME}
        gdcmMSFF
        glew
        ${GLFW_LIBRARY}
        ${OPENGL_LIBRARY}
        ${COCOA_LIBRARY}
        ${IOKIT_LIBRARY}
        ${COREVIDEO_LIBRARY}
    )
endif(APPLE)
# -----------------------------------------------------------------------------------------------------------------------