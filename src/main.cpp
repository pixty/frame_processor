/*
 * main.cpp

 *
 *  Created on: Jan 16, 2017
 *      Author: dmitry
 */

#include <boost/program_options.hpp>
#include <unistd.h>

#include "logger.hpp"
#include "model.hpp"
#include "video_streaming/webcam_video_stream.hpp"
#include "video_streaming/file_video_stream.hpp"
#include "video_streaming/image_window.hpp"

namespace po = boost::program_options;

int main(int argc, char** argv) {
	po::options_description desc("Allowed options");
	desc.add_options()
			("help,h", "Print usage message")
			("debug", po::value<bool>()->default_value(false), "Enable debug log level")
			("src_file", po::value<std::string>(), "Read frame stream from the file")
			("dst_display", po::value<bool>()->default_value(false), "Snow stream on the screen.");

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	if (vm.count("help") || vm.count("-h")) {
		std::cout << desc << "\n";
		return 1;
	}

	if (vm["debug"].as<bool>()) {
		LOG_INFO("Enabling debug option");
		debug_enabled(true);
	}

	fproc::PVideoStream src;
	std::string src_file = vm.count("src_file") ? vm["src_file"].as<std::string>() : "";
	if (src_file.length()) {
		LOG_INFO("Will read video stream from file=" << src_file);
		src = fproc::PVideoStream(new fproc::FileVideoStream(src_file, 0, -1));
	} else {
		LOG_INFO("Will read video stream from built in camera");
		src = fproc::PVideoStream(new fproc::WebcamVideoStream());
	}

	fproc::PVideoStreamConsumer dst;
	if (vm["dst_display"].as<bool>()) {
		LOG_INFO("Will show video in a window");
		dst = fproc::PVideoStreamConsumer(new fproc::ImageWindow());
	} else {
		LOG_INFO("Will not show video (null consumer)");
		dst = fproc::PVideoStreamConsumer(new fproc::VideoStreamConsumer());
	}

	fproc::VideoStreamCopier vsc(src, dst);
	vsc.process();

	return 0;
}
