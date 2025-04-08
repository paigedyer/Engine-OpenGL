find_path(glad_INCLUDE_DIR
    NAMES glad.h
    HINTS
        "${CMAKE_SOURCE_DIR}/lib/glad/include"
)
set(glad_INCLUDE_DIR ${glad_INCLUDE_DIR})

# find_library(glad_LIBRARY NAMES glad)
# set(glad_LIBRARY ${glad_LIBRARY} ${CMAKE_DL_LIBS})

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(
    glad
    REQUIRED_VARS glad_INCLUDE_DIR
)
mark_as_advanced(glad_INCLUDE_DIR)

if(glad_FOUND AND NOT TARGET glad)
    # Adds an interface library to hold the include directory
    add_library(glad INTERFACE)
    target_include_directories(glad INTERFACE "${glad_INCLUDE_DIR}"
    )
endif()

# Export the glad_SOURCE_FILE variable
if(glad_FOUND)
    set(glad_SOURCE_FILE "${CMAKE_SOURCE_DIR}/lib/glad/src/glad.c" CACHE PATH "Path to glad.c source file" FORCE)
    mark_as_advanced(glad_SOURCE_FILE)
endif()