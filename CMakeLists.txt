CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

# for CMake 2.6 corrected behaviour (see "cmake --help-policy CMP0003")
IF(
        COMMAND cmake_policy AND
        ${CMAKE_MAJOR_VERSION} EQUAL 2 AND
        ${CMAKE_MINOR_VERSION} GREATER 4
)
    CMAKE_POLICY(SET CMP0003 NEW)
    CMAKE_POLICY(SET CMP0005 NEW)
    CMAKE_POLICY(SET CMP0011 NEW)
ENDIF(
        COMMAND cmake_policy AND
        ${CMAKE_MAJOR_VERSION} EQUAL 2 AND
        ${CMAKE_MINOR_VERSION} GREATER 4
)

## ================
## = Project name =
## ================

PROJECT(segmentation)

## ============
## = Packages =
## ============

FIND_PACKAGE(ITK REQUIRED)
IF(ITK_FOUND)
    INCLUDE(${ITK_USE_FILE})
ENDIF(ITK_FOUND)

## ================================================
## = Do not allow to build inside the source tree =
## ================================================

IF(PROJECT_BINARY_DIR STREQUAL ${PROJECT_SOURCE_DIR})
    MESSAGE(FATAL_ERROR "Building in the source tree is not allowed")
ENDIF(PROJECT_BINARY_DIR STREQUAL ${PROJECT_SOURCE_DIR})

## =====================================
## = Where to put executables and libs =
## =====================================

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR})
MARK_AS_ADVANCED(
        CMAKE_BACKWARDS_COMPATIBILITY
        EXECUTABLE_OUTPUT_PATH
        LIBRARY_OUTPUT_PATH
)

## =======================
## = Programs to compile =
## =======================

ADD_EXECUTABLE(extraerImagen extraerImagen.cxx)
TARGET_LINK_LIBRARIES(extraerImagen ${ITK_LIBRARIES})

ADD_EXECUTABLE(extraerImagenSerial extraerImagenSerial.cxx)
TARGET_LINK_LIBRARIES(extraerImagenSerial ${ITK_LIBRARIES})

ADD_EXECUTABLE(filtroExtraccion filtroExtraccion.cxx)
TARGET_LINK_LIBRARIES(filtroExtraccion ${ITK_LIBRARIES})

ADD_EXECUTABLE(filtroExtraccionSerial filtroExtraccionSerial.cxx)
TARGET_LINK_LIBRARIES(filtroExtraccionSerial ${ITK_LIBRARIES})

## eof - CMakeLists.txt
