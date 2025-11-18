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
		${CMAKE_SOURCE_DIR}/lib/imgui/includes
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
		${ENGINE_SOURCE_PATH}/WindowManager.cpp
		${ENGINE_SOURCE_PATH}/InputManager.cpp
)

target_include_directories(glad PUBLIC
		${glad_INCLUDE_DIR}
)

target_include_directories(engine PUBLIC
		${ENGINE_INCLUDES_DIR}
)

set(IMGUI_SOURCE_PATH lib/imgui/src)
set(IMGUI_INCLUDES_DIR lib/imgui/includes)
add_library(imgui STATIC
		${IMGUI_SOURCE_PATH}/imgui.cpp
		${IMGUI_SOURCE_PATH}/imgui_demo.cpp
		${IMGUI_SOURCE_PATH}/imgui_draw.cpp
		${IMGUI_SOURCE_PATH}/imgui_impl_glfw.cpp
		${IMGUI_SOURCE_PATH}/imgui_impl_opengl3.cpp
		${IMGUI_SOURCE_PATH}/imgui_tables.cpp
		${IMGUI_SOURCE_PATH}/imgui_widgets.cpp
)
target_link_libraries(imgui
	glad
	${GLFW3_LIBRARY}
)
target_include_directories(imgui PUBLIC
	${IMGUI_INCLUDES_DIR}
)
target_link_libraries(engine
	imgui
)

# TILEMAP EDITOR TOOL EXE
add_executable(TOOL_tilemap_editor
		TilemapEditor/main.cpp
		TilemapEditor/TilemapEditor.cpp
)
target_link_libraries(TOOL_tilemap_editor
		engine
		glad
		${GLFW3_LIBRARY}
		OpenGL::GL
		imgui
)

target_include_directories(TOOL_tilemap_editor PUBLIC
		TilemapEditor/includes
		${IMGUI_INCLUDES_DIR}
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
		${IMGUI_INCLUDES_DIR}
)

target_link_libraries(GameEngine
	engine
	glad
	imgui
	assimp
	${GLFW3_LIBRARY} 
	OpenGL::GL

)
