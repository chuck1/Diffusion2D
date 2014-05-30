#ifndef DIFF2D_LOG_HPP
#define DIFF2D_LOG_HPP

/** @todo issue error from CMake if user #defines a log channel
 * only the library build can decide if log channels are static or dynamic
 */

/** @todo issue error is user sets a dynamic channel variable when the channel has already been defined as static */

#ifndef LOG_CORE
#    define LOG_CORE  d2d::log::core
#endif
#ifndef LOG_ARRAY
#    define LOG_ARRAY d2d::log::array
#endif


#define LOG_SEV_CHANNEL(sl_, chan_) if(sl_ >= chan_) std::cout

//We define our own severity levels
namespace d2d {
	namespace log {
		enum sl {
			debug,
			info,
			warning,
			error,
			critical
		};

		extern sl core;
		extern sl array;
	
		void init();
	}
}



#endif


