#include <Diff2D/log.hpp>

#include <cstddef>
#include <string>
#include <ostream>
#include <fstream>
#include <iomanip>

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
		"notification",
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

void		d2d::init(d2d::severity_level sl, d2d::log::flag flag) {
	// Setup the common formatter for all sinks
	logging::formatter fmt = expr::stream
		<< std::setw(6) << std::setfill('0') << line_id << std::setfill(' ')
		<< ": <" << severity << ">\t"
		<< expr::smessage;

	// Initialize sinks
	typedef sinks::synchronous_sink< sinks::text_ostream_backend > text_sink;
	
	// full
	{
		boost::shared_ptr< text_sink > sink = boost::make_shared< text_sink >();

		sink->locked_backend()->add_stream(boost::make_shared< std::ofstream >("full.log"));
		
	/*	sink->set_filter(
				severity >= sl
				&& (!expr::has_attr(tag_attr) || (tag_attr & flag))
				);
*/
		sink->set_filter(expr::has_attr< unsigned int >("Tag"));

		sink->set_formatter(fmt);

		logging::core::get()->add_sink(sink);
	}

	// important
	if(0) {
		boost::shared_ptr< text_sink > sink = boost::make_shared< text_sink >();

		sink->locked_backend()->add_stream(boost::make_shared< std::ofstream >("important.log"));

		sink->set_formatter(fmt);

		//sink->set_filter(severity >= warning || (expr::has_attr(tag_attr) && tag_attr == "IMPORTANT_MESSAGE"));

		logging::core::get()->add_sink(sink);
	}

	// Add attributes
	logging::add_common_attributes();


	lg.add_attribute("Tag", attrs::constant<unsigned int>(d2d::log::flag::array));
}


//src::severity_logger< severity_level > lg;

void		d2d::test() {

	BOOST_LOG_SEV(lg, debug) << "hello";


}





