#ifndef DIFF2D_LOG_HPP
#define DIFF2D_LOG_HPP




#ifdef LOG_DYN
#    define LOG_CORE  d2d::log::core
#    define LOG_ARRAY d2d::log::array
#    undef LOG_STA
#else
#    define LOG_STA
#endif

#ifndef LOG_CORE
#    error "must define LOG_CORE severity level or define LOG_DYN"
#endif
#ifndef LOG_ARRAY
#    error "must define LOG_ARRAY severity level or define LOG_DYN"
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


