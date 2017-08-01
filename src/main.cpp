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
#include "frame_processing/scene_detector.hpp"
#include "recognizer/recognition_manager.hpp"

namespace po = boost::program_options;

int main(int argc, char** argv) {
	po::options_description desc("Allowed options");
	desc.add_options()
			("help,h", "Print usage message")
			("debug", po::value<bool>()->default_value(false), "Enable debug log level")
			("res_path", po::value<std::string>(), "Path to the data files place.")
			("src_file", po::value<std::string>(), "Read frame stream from the file")
			("dst_display", po::value<bool>()->default_value(false), "Snow stream on the screen.")
			("scn_no_visual", po::value<bool>()->default_value(false), "Do not visualize scene detection.");

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

	using namespace fproc;
	PVideoStream src;
	std::string src_file = vm.count("src_file") ? vm["src_file"].as<std::string>() : "";
	if (src_file.length()) {
		LOG_INFO("Will read video stream from file=" << src_file);
		src = PVideoStream(new FileVideoStream(src_file, 0, -1));
	} else {
		LOG_INFO("Will read video stream from built in camera");
		src = PVideoStream(new WebcamVideoStream());
	}

	std::string res_path = vm.count("res_path") ? vm["res_path"].as<std::string>() : "./";
	LOG_INFO("Will load resource files from " << res_path);
	std::shared_ptr<DnnFaceRecognitionNet> rn(new DnnFaceRecognitionNet(res_path + "dlib_face_recognition_resnet_model_v1.dat",
			res_path + "shape_predictor_68_face_landmarks.dat"));
	PRecognitionManager rm(new RecognitionManager(rn));

	fproc::PVideoStreamConsumer dst;
	if (vm["dst_display"].as<bool>()) {
		LOG_INFO("Will show video in a window");
		dst = PVideoStreamConsumer(new ImageWindow());
	} else {
		LOG_INFO("Using standard scene detector");
		SceneDetector* sd = new SceneDetector(PSceneDetectorListener(new SceneDetectorListener()), rm);
		if (!vm["scn_no_visual"].as<bool>()) {
			LOG_INFO("Will visualize scene detection");
			sd->setVisualizer(new SceneDetectorVisualizer(*sd));
		}
		dst = PVideoStreamConsumer(sd);
	}

	fproc::VideoStreamCopier vsc(src, dst);
	vsc.process();

	return 0;
}
