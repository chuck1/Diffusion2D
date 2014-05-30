project(option_2)

add_executable(option_2 main.cpp)

FIND_PACKAGE(CMakeHelper)
FIND_PACKAGE(Diff2D)

INCLUDE(${CMakeHelper_INCLUDE_DIR}/CMakeHelper/functions.cmake)


SET(libs ${libs} Diff2D)

link_exe()


