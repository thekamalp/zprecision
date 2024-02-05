# CMakeList.txt : CMake project for zprecision, include source and define
# project specific logic here.
#

function(add_hlsl_shader SHADER_TYPE INPUT_FILE OUTPUT_FILE)
	add_custom_command(
		OUTPUT ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${OUTPUT_FILE}
		COMMAND fxc -E \"main\" -T \"${SHADER_TYPE}\" -Fo ${OUTPUT_FILE} ${CMAKE_CURRENT_SOURCE_DIR}/${INPUT_FILE}
		MAIN_DEPENDENCY ${INPUT_FILE}
		WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
	)
endfunction()

add_hlsl_shader(vs_5_0 "shaders/simple_vs.hlsl" "simple_vs.cso")
add_hlsl_shader(ps_5_0 "shaders/simple_ps.hlsl" "simple_ps.cso")

# Add source to this project's executable.
add_executable (zprecision "zprecision.cpp" "zprecision.h"
	${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/simple_vs.cso
	${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/simple_ps.cso
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET zprecision PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
target_link_libraries(zprecision k3x64)
