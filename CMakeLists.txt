cmake_minimum_required(VERSION 3.26.0)
project(GameEngine VERSION 1.0.0)
cmake_policy(SET CMP0072 NEW) # Unsure exactly what this does, but errors happen without it

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include_directories(
		${glad_INCLUDE_DIR}
		${GLFW3_INCLUDE_DIR}
		${CMAKE_SOURCE_DIR}/lib/assimp/include
		${CMAKE_SOURCE_DIR}/Engine
)

# Set the path for source file header files
set(SOURCE_INCLUDES_DIR ${CMAKE_SOURCE_DIR}/src/includes)
# Set the path for engine library files
set(ENGINE_SOURCE_PATH Engine/src)
set(ENGINE_INCLUDES_DIR ${ENGINE_SOURCE_PATH}/includes)

find_package(glad REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(assimp REQUIRED)

add_library(engine
		${ENGINE_SOURCE_PATH}/Engine.cpp
		${ENGINE_SOURCE_PATH}/stb_image.cpp
		${ENGINE_SOURCE_PATH}/Mesh.cpp
		${ENGINE_SOURCE_PATH}/Model.cpp
		${ENGINE_SOURCE_PATH}/Camera.cpp
)

target_include_directories(engine PUBLIC
		${ENGINE_INCLUDES_DIR}
)

# TILEMAP EDITOR TOOL EXE
add_executable(TOOL_tilemap_editor
		TilemapEditor/main.cpp
		TilemapEditor/TilemapEditor.cpp
)
target_link_libraries(TOOL_tilemap_editor engine)

target_include_directories(glad PUBLIC 
	${glad_INCLUDE_DIR}
)

# GAME ENGINE EXE - DEGBUG, TESTING, ETC.
add_executable(GameEngine
		${ENGINE_SOURCE_PATH}/main.cpp
)

target_include_directories(GameEngine PRIVATE
		${glad_INCLUDE_DIR}
		${GLFW3_INCLUDE_DIR}
		${ASSIMP_INCLUDE_DIRS}
		${ENGINE_INCLUDES_DIR}
)

target_link_libraries(GameEngine
	engine
	glad
	assimp
	${GLFW3_LIBRARY} 
	OpenGL::GL
)
