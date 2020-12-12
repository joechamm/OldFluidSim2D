cmake_minimum_required (VERSION 2.8)

project (OldFluidSim2D)

# Use Folders for Visual Studio
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_DEBUG_POSTFIX "_d")

# TODO: update to avoid using GLUT/GLEW
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm)

# Add shader files TODO: Is this the right way to do this?
file(GLOB_RECURSE SHADERS
	"${CMAKE_SOURCE_DIR}/shaders/*.*.glsl"
	)
	
set( HEADERS 
	"${CMAKE_SOURCE_DIR}/utils/GLSLShader.h"
	)
	
set( SRC_FILES
	${HEADERS}
	"${CMAKE_SOURCE_DIR}/utils/GLSLShader.cpp"
	"${CMAKE_SOURCE_DIR}/main.cpp"
	)
	
include_directories(
	"${CMAKE_SOURCE_DIR}/utils"
	"${CMAKE_SOURCE_DIR}/shaders"
	)

add_executable(OldFluidSim2D WIN32
	${HEADERS}
	${SRC_FILES}
	)
	
target_link_libraries(OldFluidSim2D PRIVATE
	GLEW::GLEW
	GLUT::GLUT
	)
