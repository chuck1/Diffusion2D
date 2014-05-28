cmake_minimum_required (VERSION 2.8.8)

project(Diff2D)

find_package(CMakeHelper)

# include_directories

# custom include extensions
#set(INCLUDE_EXTENSIONS obj)

#SET(GRU_VERSION 0.1.1)


# common methods for static library
# =================================
include(${CMakeHelper_INCLUDE_DIR}/CMakeHelper/static/library.cmake)


add_subdirectory(test)


