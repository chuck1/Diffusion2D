#include <iostream>

#include <Galaxy-Log/log.hpp>

#include <Diff2D/log.hpp>

void	d2d::log::init() {
	
	gal::log::min_severity["Diff2D"] = info;
	
	gal::log::init();
}




