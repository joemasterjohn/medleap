cmake_minimum_required(VERSION 2.8.12)
project(medleap)

# Directory for cmake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")


# output the executable in the build directory (not Debug/Release subdir)
foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_CURRENT_BINARY_DIR})
endforeach(OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)



# make sure resources are copied into the build directory
set(DATA_SHADERS
    shaders/color.vert
    shaders/color.frag
    shaders/slice_clut.vert
    shaders/slice_clut.frag
    shaders/volume_clut.vert
    shaders/volume_clut.frag
    shaders/histo_line.vert
    shaders/histo_line.frag
    shaders/clut_strip.vert
    shaders/clut_strip.frag
    shaders/texture_2D.vert
    shaders/texture_2D.frag
    shaders/menu.vert
    shaders/menu.frag
	shaders/color_pick_hsv_circle.vert
	shaders/color_pick_hsv_circle.frag
	shaders/color_pick_selected.vert
	shaders/color_pick_selected.frag
	shaders/color_pick_gradient.vert
	shaders/color_pick_gradient.frag
	shaders/tf1d_histo.vert
    shaders/tf1d_histo.frag
	shaders/tf1d_histo_outline.vert
	shaders/tf1d_histo_outline.frag
	shaders/orientation_cube.vert
	shaders/orientation_cube.frag
	shaders/icon.vert
	shaders/icon.frag
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



# source group naming for IDEs
source_group("shaders" FILES ${DATA_SHADERS})
source_group("fonts" FILES ${DATA_FONTS})

file(GLOB SOURCE_GL src/gl/*.cpp src/gl/*.c src/gl/*.h)
source_group("gl" FILES ${SOURCE_GL})

file(GLOB SOURCE_GL_GEOM src/gl/geom/*.cpp src/gl/geom/*.h)
source_group("gl\\geom" FILES ${SOURCE_GL_GEOM})

file(GLOB SOURCE_GL_UTIL src/gl/util/*.cpp src/gl/util/*.h)
source_group("gl\\util" FILES ${SOURCE_GL_UTIL})

file(GLOB SOURCE_GL_MATH src/gl/math/*.cpp src/gl/math/*.h)
source_group("gl\\math" FILES ${SOURCE_GL_MATH})

file(GLOB SOURCE_MAIN src/main/*.cpp src/main/*.h)
source_group("main" FILES ${SOURCE_MAIN})

file(GLOB SOURCE_DATA src/data/*.cpp src/data/*.h)
source_group("data" FILES ${SOURCE_DATA})

file(GLOB SOURCE_LEAP src/leap/*.cpp src/leap/*.h)
source_group("leap" FILES ${SOURCE_LEAP})

file(GLOB SOURCE_UTIL src/util/*.cpp src/util/*.h src/util/*.c)
source_group("util" FILES ${SOURCE_UTIL})

file(GLOB SOURCE_LAYERS src/layers/*.cpp src/layers/*.h)
source_group("layers" FILES ${SOURCE_LAYERS})

file(GLOB SOURCE_LAYER_SLICE src/layers/slice/*.cpp src/layers/slice/*.h)
source_group("layers\\slice" FILES ${SOURCE_LAYER_SLICE})

file(GLOB SOURCE_LAYER_TFN1D src/layers/transfer_1D/*.cpp src/layers/transfer_1D/*.h)
source_group("layers\\transfer_1D" FILES ${SOURCE_LAYER_TFN1D})

file(GLOB SOURCE_LAYER_TFN2D src/layers/transfer_2D/*.cpp src/layers/transfer_2D/*.h)
source_group("layers\\transfer_2D" FILES ${SOURCE_LAYER_TFN2D})

file(GLOB SOURCE_LAYER_VOLUME src/layers/volume/*.cpp src/layers/volume/*.h)
source_group("layers\\volume" FILES ${SOURCE_LAYER_VOLUME})

file(GLOB SOURCE_LAYER_VOLINFO src/layers/volume_info/*.cpp src/layers/volume_info/*.h)
source_group("layers\\volume_info" FILES ${SOURCE_LAYER_VOLINFO})

file(GLOB SOURCE_LAYER_VOLORNT src/layers/volume_orientation/*.cpp src/layers/volume_orientation/*.h)
source_group("layers\\volume_orientation" FILES ${SOURCE_LAYER_VOLORNT})

file(GLOB SOURCE_LAYER_MENU src/layers/menu/*.cpp src/layers/menu/*.h)
source_group("layers\\menu" FILES ${SOURCE_LAYER_MENU})

file(GLOB SOURCE_LAYER_COLORPICK src/layers/color_pick/*.cpp src/layers/color_pick/*.h)
source_group("layers\\color_pick" FILES ${SOURCE_LAYER_COLORPICK})

file(GLOB SOURCE_LAYER_ORIENTATION src/layers/orientation/*.cpp src/layers/orientation/*.h)
source_group("layers\\orientation" FILES ${SOURCE_LAYER_ORIENTATION})

file(GLOB SOURCE_LAYER_CLIP src/layers/clip/*.cpp src/layers/clip/*.h)
source_group("layers\\clip" FILES ${SOURCE_LAYER_CLIP})

file(GLOB SOURCE_LAYER_MASK src/layers/mask/*.cpp src/layers/mask/*.h)
source_group("layers\\mask" FILES ${SOURCE_LAYER_MASK})

file(GLOB SOURCE_LAYER_FOCUS src/layers/focus/*.cpp src/layers/focus/*.h)
source_group("layers\\focus" FILES ${SOURCE_LAYER_FOCUS})

file(GLOB SOURCE_LAYER_LEAP_STATE src/layers/leap_state/*.cpp src/layers/leap_state/*.h)
source_group("layers\\leap_state" FILES ${SOURCE_LAYER_LEAP_STATE})

set(SOURCE_AND_RESOURCES
    ${SOURCE_MAIN}
    ${SOURCE_DATA}
	${SOURCE_LEAP}
    ${SOURCE_LAYERS}
    ${SOURCE_LAYER_SLICE}
    ${SOURCE_LAYER_TFN1D}
    ${SOURCE_LAYER_TFN2D}
    ${SOURCE_LAYER_VOLUME}
    ${SOURCE_LAYER_VOLINFO}
    ${SOURCE_LAYER_VOLORNT}
	${SOURCE_LAYER_MENU}
	${SOURCE_LAYER_COLORPICK}
	${SOURCE_LAYER_ORIENTATION}
	${SOURCE_LAYER_CLIP}
	${SOURCE_LAYER_MASK}
	${SOURCE_LAYER_FOCUS}
	${SOURCE_LAYER_LEAP_STATE}
    ${SOURCE_GL}
	${SOURCE_GL_MATH}
    ${SOURCE_GL_GEOM}
    ${SOURCE_GL_UTIL}
    ${SOURCE_UTIL}
    ${DATA_SHADERS}
    ${DATA_FONTS}
)

# set root of source directory for finding headers
include_directories(src)

# Find the libraries (must have LEAP_DIR and possibly GDCM_DIR/GLFW_DIR env variables)
find_package(GDCM CONFIG REQUIRED)
include_directories(${GDCM_INCLUDE_DIRS})
find_package(GLFW CONFIG REQUIRED)
include_directories(${GLFW_INCLUDE_DIR})
find_package(Leap REQUIRED)
include_directories(${LEAP_INCLUDE_DIR})

message(STATUS ${LEAP_INCLUDE_DIR})

if (APPLE)

    add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${SOURCE_AND_RESOURCES})

    # Copy resources to the right place in the app bundle
    SET_SOURCE_FILES_PROPERTIES(
        ${DATA_SHADERS} 
        PROPERTIES 
        MACOSX_PACKAGE_LOCATION
        Resources/shaders
    )
    SET_SOURCE_FILES_PROPERTIES(
        ${DATA_FONTS} 
        PROPERTIES 
        MACOSX_PACKAGE_LOCATION
        Resources/fonts
    )

    # for C++11
    set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")

    # GLFW needs OpenGL, Cocoa, IOKit, and CoreVideo frameworks linked
    find_library(OPENGL_LIBRARY OpenGL)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COREVIDEO_LIBRARY CoreVideo)

    # Link all the libraries
    target_link_libraries(
        ${PROJECT_NAME}
        gdcmMSFF
        ${GLFW_LIBRARY}
        ${OPENGL_LIBRARY}
        ${COCOA_LIBRARY}
        ${IOKIT_LIBRARY}
        ${COREVIDEO_LIBRARY}
        ${LEAP_LIBRARY}
    )

    # Copy libLeap.dylib to output directory
    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} ARGS -E copy
        ${LEAP_LIBRARY}
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )

endif(APPLE)

if (WIN32)
    add_executable(${PROJECT_NAME} ${SOURCE_AND_RESOURCES})

	# is this still needed? check
	add_definitions(-DGLEW_STATIC)

    target_link_libraries(
        ${PROJECT_NAME}
        gdcmMSFF
		opengl32
        ${GLFW_LIBRARY}
    )

    # use Leap.lib for RELEASE or Leapd.lib for DEBUG
    target_link_libraries (${PROJECT_NAME} optimized ${LEAP_LIB_RELEASE} debug ${LEAP_LIB_DEBUG})

    # copy Leap.dll for RELEASE or Leapd.dll for DEBUG into output directory
    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} ARGS -E copy
        "$<$<CONFIG:debug>:${LEAP_DLL_DEBUG}>$<$<CONFIG:release>:${LEAP_DLL_RELEASE}>"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endif(WIN32)

# Ensure shaders are copied whenever modified
add_dependencies(${PROJECT_NAME} SHADER_COPY)