## Find source and headers
FILE(GLOB HEADERS source/*.h)
FILE(GLOB SOURCES source/*.c*)

IF(CMAKE_HAS_SORT)
  LIST(SORT HEADERS)
  LIST(SORT SOURCES)
ENDIF(CMAKE_HAS_SORT)

## Build
INCLUDE_DIRECTORIES(
  source
)

ADD_LIBRARY(tinycthread ${SOURCES} ${HEADERS})
