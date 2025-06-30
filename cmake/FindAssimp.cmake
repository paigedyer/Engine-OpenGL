set(ASSIMP_INCLUDE_DIRS
"${CMAKE_SOURCE_DIR}/lib/assimp/include"
)
set(assimp_LIB_SEARCH_DIRS
"${CMAKE_SOURCE_DIR}/lib/assimp/assimp"
)
# Locate .lib file
find_library(ASSIMP_LIBRARY NAMES assimp assimp-vc143-mtd
PATHS ${assimp_LIB_SEARCH_DIRS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(assimp DEFAULT_MSG
ASSIMP_LIBRARY
)

if(assimp_FOUND AND NOT TARGET assimp)
    add_library(assimp SHARED IMPORTED)

    set_target_properties(assimp PROPERTIES
            IMPORTED_LOCATION "${assimp_LIB_SEARCH_DIRS}/assimp-vc143-mtd.dll"
            IMPORTED_IMPLIB "${ASSIMP_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${ASSIMP_INCLUDE_DIRS}"
    )
endif()