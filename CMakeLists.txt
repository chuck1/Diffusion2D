cmake_minimum_required (VERSION 2.8.8)

project(Diff2D)

find_package(CMakeHelper)

#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -dM")

# include_directories

# custom include extensions
#set(INCLUDE_EXTENSIONS obj)

#SET(GRU_VERSION 0.1.1)

ADD_DEFINITIONS("-DDEBUG -DLOG_DYN")


# common methods for static library
# =================================
include(${CMakeHelper_INCLUDE_DIR}/CMakeHelper/static/library.cmake)


add_subdirectory(test)


