add_executable(test1 main.cpp)

FIND_PACKAGE(Diff2D)

MESSAGE("d2d libs: ${Diff2D_LIBRARIES}")

target_link_libraries(test1 ${Diff2D_LIBRARIES})

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-use-linker-plugin")

