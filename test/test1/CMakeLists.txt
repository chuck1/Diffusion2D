add_executable(test1 main.cpp)

FIND_PACKAGE(Diff2D)

target_link_libraries(test1 ${Diff2D_LIBRARIES})


