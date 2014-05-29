add_executable(array main.cpp)

FIND_PACKAGE(Diff2D)

target_link_libraries(array ${Diff2D_LIBRARIES})


