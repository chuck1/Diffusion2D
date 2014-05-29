add_executable(test1 main.cpp)

#FIND_PACKAGE(Diff2D)

ADD_DEFINITIONS("-DBOOST_ALL_DYN_LINK")

#SET(Boost_USE_STATIC_LIBS ON)


SET(BOOST_ROOT /scratch/usr)
SET(BOOST_LIBRARY_DIR /scratch/usr/lib)

SET(Boost_NO_SYSTEM_PATHS ON)

set(Boost_USE_STATIC_LIBS OFF)
ADD_DEFINITIONS("-DBOOST_ALL_DYN_LINK")

set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)


FIND_PACKAGE(
	Boost 1.55
	COMPONENTS log system thread
	REQUIRED)


MESSAGE("boost: ${Boost_LOG_LIBRARY}")
MESSAGE("boost: ${Boost_SYSTEM_LIBRARY}")
MESSAGE("boost: ${Boost_THREAD_LIBRARY}")



SET(libs ${libs}
	Diff2D
	${Boost_LOG_LIBRARY}
	${Boost_SYSTEM_LIBRARY}
	${Boost_THREAD_LIBRARY}
	pthread
	rt
	)




SET(libs ${libs} ${libs})

MESSAGE(STATUS "libs: ${libs}")
message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}")

#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-use-linker-plugin")

target_link_libraries(test1 ${libs})


