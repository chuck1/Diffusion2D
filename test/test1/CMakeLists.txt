add_executable(test1 main.cpp)

SET(Boost_USE_STATIC_LIBS ON)
FIND_PACKAGE(Boost 1.54 COMPONENTS log REQUIRED)

SET(libs Diff2D pthread rt ${Boost_LOG_LIBRARY} boost_log boost_system boost_thread)

target_link_libraries(test1 ${libs})


