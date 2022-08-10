INCLUDE(ExternalProject)

IF(WIN32 AND MSVC)
    MESSAGE(STATUS "fltk build skipped on windows platform, use vcpkg")
    RETURN()
ENDIF()

ExternalProject_Add(
    fltk

    GIT_REPOSITORY "https://github.com/etorth/fltk.git"

    SOURCE_DIR "${MIR2X_3RD_PARTY_DIR}/fltk"
    BUILD_IN_SOURCE TRUE

    CONFIGURE_COMMAND ""
    BUILD_COMMAND make
    INSTALL_COMMAND make install prefix=${MIR2X_3RD_PARTY_DIR}/fltk/install
    UPDATE_COMMAND ""
    PATCH_COMMAND ""

    LOG_BUILD 1
    LOG_CONFIGURE 1
    LOG_INSTALL 1
)

SET(FLTK_INCLUDE_DIRS "${MIR2X_3RD_PARTY_DIR}/fltk/install/include")
IF(WIN32)
    SET(FLTK_LIBRARIES fltk_static)
ELSE()
    SET(FLTK_LIBRARIES
        "${CMAKE_STATIC_LIBRARY_PREFIX}fltk${CMAKE_STATIC_LIBRARY_SUFFIX}"
        "${CMAKE_STATIC_LIBRARY_PREFIX}fltk_gl${CMAKE_STATIC_LIBRARY_SUFFIX}"
        "${CMAKE_STATIC_LIBRARY_PREFIX}fltk_forms${CMAKE_STATIC_LIBRARY_SUFFIX}"
        "${CMAKE_STATIC_LIBRARY_PREFIX}fltk_images${CMAKE_STATIC_LIBRARY_SUFFIX}")
ENDIF()

INCLUDE_DIRECTORIES(SYSTEM ${FLTK_INCLUDE_DIRS})
LINK_DIRECTORIES(${MIR2X_3RD_PARTY_DIR}/fltk/install/lib)
ADD_DEPENDENCIES(mir2x_3rds fltk)
