/*
 * main.cpp

 *
 *  Created on: Jan 16, 2017
 *      Author: dmitry
 */

#include <boost/program_options.hpp>

#include "logger.hpp"

#include "frame_processing/show_stream_detector.hpp"
#include "frame_processing/vfile_scene_detector.hpp"
#include "video_streaming/webcam_video_stream.hpp"
#include "video_streaming/file_video_stream.hpp"
#include "frame_processing/naive_scene_detector.hpp"
#include "frame_processing/naive_scene_detector_debugger.hpp"

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
	//fproc::WebcamVideoStream wcvs;    
	fproc::FileVideoStream fvs("video-test.mkv", false);
	//fproc::VFileSceneDetector ssd(fvs, "/Users/dmitry/Downloads/pixty2.avi"); // It should be an avi file with mjpeg codec
	//fproc::ShowStreamDetector ssd(fvs);
	//ssd.process();
	fproc::NaiveSceneDetector::PDebugger dbg(new fproc::NaiveSceneDetectorDebugger());
	fproc::NaiveSceneDetector nsd(fvs, fproc::nil_sc_detecor_listener, dbg, fproc::NaiveSceneDetectorParameters(60,160));
	nsd.process();
	return 0;
}
