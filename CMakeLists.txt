project(specter)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake-modules")

file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)
set(QT_HEADERS src/specter_widget.h)
file(GLOB_RECURSE QT_FORMS ${CMAKE_SOURCE_DIR}/src/*.ui)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost 1.55 REQUIRED system thread)
find_package(Jack REQUIRED)
find_package(Qt4 REQUIRED)

qt4_wrap_cpp(QT_HEADERS_MOC ${QT_HEADERS})
qt4_wrap_ui(QT_FORMS_HEADERS ${QT_FORMS})

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${BOOST_INCLUDE_DIRS})

add_executable(specter
  ${SOURCES}
  ${QT_HEADERS_MOC}
  ${QT_FORMS_HEADERS})
target_link_libraries(specter
  ${JACK_LIBRARIES}
  ${QT_LIBRARIES}
  ${Boost_LIBRARIES})
