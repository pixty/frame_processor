/*
 * main.cpp

 *
 *  Created on: Jan 16, 2017
 *      Author: dmitry
 */

#include <boost/program_options.hpp>

#include "logger.hpp"
#include <list>

namespace po = boost::program_options;

// This is just for the example so far...
extern int camera_should_move2();

class MyObject{};

int main(int argc, char** argv) {
	po::options_description desc("Allowed options");
	desc.add_options()
	    ("help,h", "print usage message")
	    ("debug", po::value<bool>()->default_value(false), "enable debug log level")
        ("source", po::value<bool>()->default_value(false), "source video file for testing");
	;

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	if (vm.count("help") || vm.count("h")) {
	    std::cout << desc << "\n";
	    return 1;
	}

	if (vm["debug"].as<bool>()) {
		LOG_INFO("Enabling debug option");
		debug_enabled(true);
	}
    return camera_should_move2();
}
