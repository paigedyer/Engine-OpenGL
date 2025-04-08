cmake_minimum_required(VERSION 3.26.0)
project(GameEngine VERSION 1.0.0)
cmake_policy(SET CMP0072 NEW) # Unsure exactly what this does, but errors happen without it

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include_directories(${CMAKE_SOURCE_DIR}/lib)

# Uncomment this when ready
find_package(glad REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(GameEngine
	src/main.cpp
)

target_include_directories(GameEngine PRIVATE 
	${glad_INCLUDE_DIR}
	${GLFW3_INCLUDE_DIR}
)

target_include_directories(glad PUBLIC 
	${glad_INCLUDE_DIR}
)

target_link_libraries(GameEngine 
	glad 
	${GLFW3_LIBRARY} 
	OpenGL::GL
)

