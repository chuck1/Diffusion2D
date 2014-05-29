#include <iostream>

#include <Diff2D/log.hpp>



#ifdef LOG_STA
d2d::log::sl d2d::log::core  = LOG_CORE;
d2d::log::sl d2d::log::array = LOG_ARRAY;
#elif LOG_DYN
d2d::log::sl d2d::log::core  = d2d::log::info;
d2d::log::sl d2d::log::array = d2d::log::info;
#endif


void	d2d::log::init() {


	//std::cout << "sl for core is " << d2d::log::chan::core << " is " << d2d::log::global_sl[d2d::log::chan::core] << std::endl;
	//std::cout << "sl for array is" << d2d::log::chan::array << " is " << d2d::log::global_sl[d2d::log::chan::array] << std::endl;

}




