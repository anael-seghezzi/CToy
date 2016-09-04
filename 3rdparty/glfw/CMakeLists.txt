## Find source and headers

FILE(GLOB HEADERS
  include/GLFW/glfw3.h
  include/GLFW/glfw3native.h
)

FILE(GLOB SOURCES
  src/context.c
  src/init.c
  src/input.c
  src/monitor.c
  src/vulkan.c  
  src/window.c
)

IF(WIN32)
  FILE(GLOB OS_HEADERS src/win32_*.h src/wgl_*.h egl_context.h)
  FILE(GLOB OS_SOURCES src/win32_*.c src/wgl_*.c egl_context.c)
  ADD_DEFINITIONS(-D_GLFW_WIN32)
ENDIF(WIN32)

IF(APPLE)
  FILE(GLOB OS_HEADERS
    src/cocoa_platform.h
    src/cocoa_joystick.h
    src/posix_tls.h
    src/nsgl_context.h
  )
  FILE(GLOB OS_SOURCES
    src/cocoa_init.m
    src/cocoa_joystick.m
    src/cocoa_monitor.m
    src/cocoa_window.m
    src/cocoa_time.c
    src/posix_tls.c
    src/nsgl_context.m
  )
  ADD_DEFINITIONS(-D_GLFW_COCOA)
  set_source_files_properties(${OS_SOURCES} PROPERTIES LANGUAGE C)
ENDIF(APPLE)

IF(UNIX AND NOT APPLE)
  FILE(GLOB OS_HEADERS src/x11_*.h src/glx_*.h)
  FILE(GLOB OS_SOURCES src/x11_*.c src/glx_*.c)
  ADD_DEFINITIONS(-D_GLFW_X11)
  ADD_DEFINITIONS(-D_GLFW_GLX)
  ADD_DEFINITIONS(-D_GLFW_HAS_GLXGETPROCADDRESS)
ENDIF(UNIX AND NOT APPLE)

IF(CMAKE_HAS_SORT)
  LIST(SORT HEADERS)
  LIST(SORT SOURCES)
  LIST(SORT OS_HEADERS)
  LIST(SORT OS_SOURCES)
ENDIF(CMAKE_HAS_SORT)

## Build
ADD_DEFINITIONS(-D_GLFW_USE_MENUBAR)
ADD_DEFINITIONS(-D_GLFW_USE_CHDIR)

INCLUDE_DIRECTORIES(
  include
)

ADD_LIBRARY(glfw ${SOURCES} ${OS_HEADERS} ${OS_SOURCES})
