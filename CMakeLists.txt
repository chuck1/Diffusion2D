cmake_minimum_required (VERSION 2.8.8)

project(Diff2D)

find_package(CMakeHelper)

# include_directories

# custom include extensions
#set(INCLUDE_EXTENSIONS obj)

#SET(GRU_VERSION 0.1.1)

ADD_DEFINITIONS("-DDEBUG")
ADD_DEFINITIONS("-DBOOST_LOG_DYN_LINK")


#SET(Boost_USE_STATIC_LIBS ON)

FIND_PACKAGE(Boost 1.55
	COMPONENTS log program_options system thread
	REQUIRED)

MESSAGE("boost: ${Boost_LOG_LIBRARY}")
MESSAGE("boost: ${Boost_PROGRAM_OPTIONS_LIBRARY}")
MESSAGE("boost: ${Boost_SYSTEM_LIBRARY}")
MESSAGE("boost: ${Boost_THREAD_LIBRARY}")



SET(libs ${libs}
	${Boost_LOG_LIBRARY}
	${Boost_PROGRAM_OPTIONS_LIBRARY}
	${Boost_SYSTEM_LIBRARY}
	${Boost_THREAD_LIBRARY}
	pthread
	rt
	)


#SET(libs ${libs} boost_system boost_thread)


SET(libs ${libs} ${libs})


# common methods for static library
# =================================
include(${CMakeHelper_INCLUDE_DIR}/CMakeHelper/static/library.cmake)


add_subdirectory(test)


