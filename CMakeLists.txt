cmake_minimum_required(VERSION 2.6)

find_package(Qt4 4.5.0 REQUIRED QtCore QtGui)
include(${QT_USE_FILE})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Woverloaded-virtual -Wall -Wno-sign-compare -Werror")

set(SOURCES
  demo.cpp
  main.cpp
  nyanproxystyle.cpp
)

set(HEADERS
  demo.h
  nyanproxystyle.h
)

set(UI
  demo.ui
)

set(RESOURCES
  data.qrc
)

qt4_wrap_cpp(MOC ${HEADERS})
qt4_wrap_ui(UIC ${UI})
qt4_add_resources(QRC ${RESOURCES})

add_executable(demo
  ${SOURCES}
  ${MOC}
  ${UIC}
  ${QRC}
)

target_link_libraries(demo
  ${QT_LIBRARIES}
)

