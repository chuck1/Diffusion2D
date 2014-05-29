cmake_minimum_required (VERSION 2.8.8)

project(Diff2D)

find_package(CMakeHelper)

# include_directories

# custom include extensions
#set(INCLUDE_EXTENSIONS obj)

#SET(GRU_VERSION 0.1.1)

ADD_DEFINITIONS("-DDEBUG")


SET(Boost_USE_STATIC_LIBS ON)
FIND_PACKAGE(Boost 1.55 COMPONENTS log program_options REQUIRED)

MESSAGE("boost log: ${Boost_LOG_LIBRARY}")
MESSAGE("boost log: ${Boost_PROGRAM_OPTIONS_LIBRARY}")

SET(libs Diff2D pthread rt ${Boost_LOG_LIBRARY} ${Boost_PROGRAM_OPTIONS_LIBRARY} boost_system boost_thread)


# common methods for static library
# =================================
include(${CMakeHelper_INCLUDE_DIR}/CMakeHelper/static/library.cmake)


add_subdirectory(test)


