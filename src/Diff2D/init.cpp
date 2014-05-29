/*
import sys
import argparse

import prob


def init():
arser = argparse.ArgumentParser()
parser.add_argument('-v', '--verbose', action='store_true')
args = parser.parse_args()

if args.verbose: logging.basicConfig(level=logging.DEBUG)
d

*/

#include <Diff2D/log.hpp>

#include <boost/program_options.hpp>

int init(int ac, char** av) {

	namespace po = boost::program_options;

	// Declare the supported options.
	po::options_description desc("Allowed options");

	desc.add_options()
		("help,h", "produce help message")
		("debug,d", "debug")
		("debug-array,a", "debug array")
		;

	po::variables_map vm;
	po::store(po::parse_command_line(ac, av, desc), vm);
	po::notify(vm);    

	if (vm.count("help")) {
		std::cout << desc << std::endl;
		return 1;
	}
	
	d2d::severity_level sl = d2d::severity_level::info;
	
	d2d::log::flag flag = (d2d::log::flag)0;

	if (vm.count("debug")) {
		std::cout << "Debug messages on" << std::endl;
		sl = d2d::severity_level::debug;
	}

	if (vm.count("debug-array")) {
		std::cout << "Debug array messages on" << std::endl;
		flag = (d2d::log::flag)(flag | d2d::log::flag::array);
	}
	
	init(sl,flag);	

	return 0;
}



