cmake_minimum_required (VERSION 2.8.8)

ADD_SUBDIRECTORY(math-array)

INCLUDE($ENV{HOME}/.config.cmake)

project(Diff2D)
SET(Diff2D_STATIC ON)

find_package(CMakeHelper)
find_package(galaxy_log)
find_package(math-array)

#cmh_vars_from_file("${PROJECT_BINARY_DIR}/vars.txt")


cmh_static_library()

#cmh_process_debug(DEBUG DEBUG_FACE)

add_subdirectory(test)



