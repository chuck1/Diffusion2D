

project(test1)

FIND_PACKAGE(CMakeHelper)
FIND_PACKAGE(Diff2D)


SET(libs ${libs}
	Diff2D
	)

ADD_DEFINITIONS("-D_DEBUG")

link_exe()




