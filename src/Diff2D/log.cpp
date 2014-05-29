#include <Diff2D/log.hpp>

#include <cstddef>
#include <string>
#include <ostream>
#include <fstream>
#include <iomanip>

#include <boost/log/utility/setup/console.hpp>

//#include <boost/shared_ptr.hpp>

//#include <boost/make_shared.hpp>
//#include <boost/phoenix/bind.hpp>
//#include <boost/date_time/posix_time/posix_time.hpp>
//#include <boost/log/core.hpp>
//#include <boost/log/expressions.hpp>
//#include <boost/log/attributes.hpp>
//#include <boost/log/sources/basic_logger.hpp>
//#include <boost/log/sources/severity_logger.hpp>
//#include <boost/log/sources/severity_channel_logger.hpp>
//#include <boost/log/sources/record_ostream.hpp>
//#include <boost/log/sinks/sync_frontend.hpp>
//#include <boost/log/sinks/text_ostream_backend.hpp>
//#include <boost/log/attributes/scoped_attribute.hpp>
//#include <boost/log/utility/value_ref.hpp>
//#include <boost/log/utility/setup/common_attributes.hpp>


// The operator puts a human-friendly representation of the severity level to the stream
std::ostream& operator<< (std::ostream& strm, d2d::severity_level level)
{
	static const char* strings[] =
	{
		"debug",
		"info",
		"warning",
		"error",
		"critical"
	};

	if (static_cast< std::size_t >(level) < sizeof(strings) / sizeof(*strings)) {
		strm << strings[level];
	} else {
		strm << static_cast< int >(level);
	}

	return strm;
}

void		d2d::init(d2d::severity_level sl) {


	// Create a minimal severity table filter
	typedef expr::channel_severity_filter_actor< std::string, severity_level > min_severity_filter;
	min_severity_filter min_severity = expr::channel_severity_filter(channel, severity);

	// Set up the minimum severity levels for different channels`
	min_severity["core"] = d2d::info;
	min_severity["array"] = d2d::warning;

	logging::add_console_log
		(
		 std::clog,
		 keywords::filter = min_severity || severity >= critical,
		 keywords::format =
		 (
		  expr::stream
		  << line_id
		  << ": <" << severity
		  << "> [" << channel << "] "
		  << expr::smessage
		 )
		);

}

typedef src::severity_channel_logger< d2d::severity_level, std::string > logger_type;
logger_type lg;





