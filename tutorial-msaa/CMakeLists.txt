
set(TARGET_NAME "msaa")
##===========================================================================##

file( GLOB SHADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/*.glsl )
file( COPY ${SHADER_FILES} DESTINATION ${TUTORIAL_BINARY_DIR}/shaders/ )


file(GLOB SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

group_files("${SOURCE_FILES}")
group_files("${SHADER_FILES}")


add_executable( ${TARGET_NAME} ${COMMON_SOURCE_FILES} ${EXT_SOURCE_FILES} ${SOURCE_FILES} ${SHADER_FILES} )
target_link_libraries( ${TARGET_NAME} ${ALL_LIBS} )
set_property(TARGET ${TARGET_NAME} PROPERTY CXX_STANDARD 17)
set_target_properties(${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${TUTORIAL_BINARY_DIR})
set_target_properties(${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${TUTORIAL_BINARY_DIR})
set_target_properties(${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${TUTORIAL_BINARY_DIR})
set_target_properties(${TARGET_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${TUTORIAL_BINARY_DIR})
set_target_properties(${TARGET_NAME} PROPERTIES DEBUG_POSTFIX "_d")
set_target_properties(${TARGET_NAME} PROPERTIES RELWITHDEBINFO_POSTFIX "RelWithDebInfo")

foreach(shader ${SHADER_FILES})
    set_source_files_properties( ${shader} PROPERTIES HEADER_FILE_ONLY TRUE )
    #add_custom_command( TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${shader} shaders/ )
    add_custom_command( TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${shader} ${TUTORIAL_BINARY_DIR}/shaders/ )
    message(STATUS "copy ${shader} into ${TUTORIAL_BINARY_DIR}/shaders") 
endforeach()

