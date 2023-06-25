cmake_minimum_required(VERSION 3.10)

project(accel-utf8 CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)

if(NOT DEFINED ACCEL_MODULES_FOLDER)
    set(ACCEL_MODULES_FOLDER "${CMAKE_SOURCE_DIR}/external")
endif()

set(ADDITIONAL_LIBRARIES accel-macros)
set(ADDITIONAL_INCLUDES "")
set(ADDITIONAL_SOURCES "")

include(cmake/FindModule.cmake)
add_accel_module(accel-macros)

file(GLOB_RECURSE SRC_FILES "src/*.cpp" "include/accel/*.hpp")

add_library(accel-utf8 INTERFACE)
target_include_directories(accel-utf8 INTERFACE "include/" ${ADDITIONAL_INCLUDES})
target_link_libraries(accel-utf8 INTERFACE ${ADDITIONAL_LIBRARIES})

if(ACCEL_BUILD_TESTS)
    add_subdirectory(tests)
endif() 