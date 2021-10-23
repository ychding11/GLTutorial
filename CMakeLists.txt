cmake_minimum_required (VERSION 2.6)

project (Tutorial-OGL)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo" CACHE STRING "Configuration types" FORCE)

set(TUTORIAL_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin CACHE STRING "Tutorial Binary Dir" FORCE) #force update even if it in cache
message(STATUS "TUTORIAL_BINARY_DIR='${TUTORIAL_BINARY_DIR}' ")

find_package (OpenGL REQUIRED)

add_subdirectory (external)

option(VERBOSE_MODE "cmake verbose mode" ON)

#######################################################################################
## function definition
#######################################################################################
function(group_files src_files)
    foreach(f ${src_files})
        # replace "a/b/c" --> "a\\b\\c"
        string(REPLACE / \\\\ f ${f})
        # Get the path of the file relative to ${CMAKE_SOURCE_DIR}
        file(RELATIVE_PATH relative_path ${CMAKE_SOURCE_DIR} ${f})
        # Extract the folder, remove the filename from path 
        string(REGEX REPLACE "(.*)(/[^/]*)$" "\\1" relative_path ${relative_path})

        set(SRCGR "${relative_path}")
        # Source_group expects \\ (double antislash), not / (slash)
        string(REPLACE / \\\\ SRCGR ${SRCGR})
        #message(STATUS "1. group name=${SRCGR}, file=${f}")

        # regular expression https://cmake.org/cmake/help/latest/command/string.html#regex-specification
        if ("${f}" MATCHES ".*\\.glsl$")
           set(SRCGR "${SRCGR}\\\\glsl")
        endif()    

        if (VERBOSE_MODE)
           message(STATUS "2. group name=${SRCGR}, file=${f}")
        endif (VERBOSE_MODE)

        source_group(${SRCGR} FILES ${f})
    endforeach()
endfunction()

include_directories(
	.
	common/
	external/glew-1.13.0/include/
	external/glfw-3.1.2/include/
	external/glm-0.9.7.1/
    ${CMAKE_CURRENT_SOURCE_DIR}/external/spdlog/include
)

include_directories( ${CMAKE_SOURCE_DIR}/external/imgui )
include_directories( ${CMAKE_SOURCE_DIR}/external/fonts )

set(ALL_LIBS ${OPENGL_LIBRARY} glfw GLEW_1130)

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)

#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Model/Suzanne.obj DESTINATION Model/)
file( GLOB TEXTURE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/textures/*.jpg )
file( COPY ${TEXTURE_FILES} DESTINATION ${TUTORIAL_BINARY_DIR}/textures/)

file(GLOB Model_3D_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Model/*.obj
    ${CMAKE_CURRENT_SOURCE_DIR}/Model/*.mtl
)
file(COPY ${Model_3D_FILES}  DESTINATION ${TUTORIAL_BINARY_DIR}/Model/)
message(STATUS "copy 3d models into ${TUTORIAL_BINARY_DIR}/Model")

foreach(file ${Model_3D_FILES})   
endforeach()

file(GLOB COMMON_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/common/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/common/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/common/*.cpp
)

file(GLOB EXT_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/*.cpp
)
group_files("${COMMON_SOURCE_FILES}")
group_files("${EXT_SOURCE_FILES}")

##===========================================================================##

add_subdirectory(tutorial-double-side-shading)
