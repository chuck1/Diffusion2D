#ifndef DIFF2D_LOG_HPP
#define DIFF2D_LOG_HPP


//#include <cstddef>
//#include <string>
//#include <ostream>
//#include <fstream>
//#include <iomanip>

//#include <boost/shared_ptr.hpp>
//#include <boost/make_shared.hpp>
//#include <boost/phoenix/bind.hpp>
//#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/expressions/keyword.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/sources/basic_logger.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/attributes/scoped_attribute.hpp>
#include <boost/log/utility/value_ref.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace sinks = boost::log::sinks;
namespace attrs = boost::log::attributes;
namespace keywords = boost::log::keywords;

//We define our own severity levels
namespace d2d {
	enum severity_level
	{
		debug,
		info,
		warning,
		error,
		critical
	};
}

// The operator puts a human-friendly representation of the severity level to the stream
std::ostream& operator<< (std::ostream& strm, d2d::severity_level level);

//[ example_tutorial_filtering
BOOST_LOG_ATTRIBUTE_KEYWORD(line_id, "LineID", unsigned int)
BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", d2d::severity_level)
BOOST_LOG_ATTRIBUTE_KEYWORD(channel, "Channel", std::string)


namespace d2d {
	void init(d2d::severity_level sl);

	static src::severity_logger< d2d::severity_level > lg;

	void test();
}

#endif


