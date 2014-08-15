cmake_minimum_required (VERSION 2.8.8)

INCLUDE($ENV{HOME}/.config.cmake)

project(Diff2D)
SET(Diff2D_STATIC ON)

find_package(CMakeHelper)

find_package(math-array)

# include_directories

# submodule
#INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/math-array/include")

cmh_vars_from_file("${PROJECT_BINARY_DIR}/vars.txt")


# custom include extensions
#set(INCLUDE_EXTENSIONS obj)

#SET(GRU_VERSION 0.1.1)

cmh_library()

#cmh_process_debug(DEBUG DEBUG_FACE)

add_subdirectory(test)



