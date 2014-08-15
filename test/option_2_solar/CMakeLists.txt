project(option_2_solar)


FIND_PACKAGE(CMakeHelper)
FIND_PACKAGE(Diff2D)



SET(libs ${libs}
	Diff2D
	)



link_exe()


