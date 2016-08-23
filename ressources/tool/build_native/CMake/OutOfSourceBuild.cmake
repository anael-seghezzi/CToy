# Disallow in-source build
STRING(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" BUILDING_IN_SOURCE)
IF(BUILDING_IN_SOURCE)
  MESSAGE(FATAL_ERROR "This project requires an out of source build. Please create a separate build directory and run 'cmake [options] <path-to-source>' there.")
ENDIF(BUILDING_IN_SOURCE)

