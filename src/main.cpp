/*
 * main.cpp

 *
 *  Created on: Jan 16, 2017
 *      Author: dmitry
 */

#include <boost/program_options.hpp>

#include "logger.hpp"
#include <list>

#include "frame_processing/show_stream_detector.hpp"
#include "video_streaming/webcam_video_stream.hpp"


namespace po = boost::program_options;

int main(int argc, char** argv) {
	po::options_description desc("Allowed options");
	desc.add_options()
	    ("help,h", "print usage message")
	    ("debug", po::value<bool>()->default_value(false), "enable debug log level")
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

	fproc::WebcamVideoStream wcvs;
	fproc::ShowStreamDetector ssd(wcvs);
	ssd.process();

    return 0;
}
