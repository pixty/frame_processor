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
#include "frame_processing/fpcp_connector.hpp"
#include "recognizer/recognition_manager.hpp"

namespace po = boost::program_options;

int main(int argc, char** argv) {
	po::options_description desc("Allowed options");
	desc.add_options()
			("help,h", "Print usage message")
			("debug", po::value<bool>()->default_value(false), "Enable debug log level")
			("res_path", po::value<std::string>(), "Path to the data files place.")
			("src_file", po::value<std::string>(), "Read frame stream from the file")
			("sp_address", po::value<std::string>()->default_value("localhost:50051"), "Address for connecting to Service provider.")
			("access_key", po::value<std::string>(), "Authentication access key")
			("secret_key", po::value<std::string>(), "Authentication secret key")
			("dst_display", po::value<bool>()->default_value(false), "Don't try to detect, but snow camera video-stream on the screen.")
            ("scn_no_visual", po::value<bool>()->default_value(false), "Do not visualize scene detection (no video scene on the screen).")
            ("hog_width", po::value<int>()->default_value(-1), "Width of HOG's frame")
            ("hog_height", po::value<int>()->default_value(-1), "Height of HOG's frame")
            ("hog_grayscale", po::value<bool>()->default_value(false), "HOG uses grayscaled frames")
            ("cam_width", po::value<int>()->default_value(-1), "Width of camera's frame")
            ("cam_height", po::value<int>()->default_value(-1), "Height of camera's frame")
            ("cam_fps", po::value<int>()->default_value(-1), "Camera's FPS")
            ("cam_brightness", po::value<int>()->default_value(-1), "Camera's brightness (int): min=0 max=255 step=1 default=128")
            ("cam_contrast", po::value<int>()->default_value(-1), "Camera's contrast (int): min=0 max=255 step=1 default=128")
            ("cam_saturation", po::value<int>()->default_value(-1), "Camera's saturation (int): min=0 max=255 step=1 default=128")
            ("cam_gain", po::value<int>()->default_value(-1), "Camera's gain (int): min=0 max=255 step=1 default=0")
            ("cam_exposure_auto", po::value<int>()->default_value(-1), "Camera's exposure_auto (menu): min=0 max=3 default=3 value=3")
            ("cam_exposure_absolute", po::value<int>()->default_value(-1), "Camera's exposure_absolute (int): min=3 max=2047 step=1 default=250")
            ("cam_autofocus", po::value<bool>()->default_value(true), "Camera's focus_auto (bool): default=1")
            ("cam_fourcc", po::value<std::string>()->default_value(""), "Camera's fourcc, for instance MJPG, YUYV, H264")
            //TODO:  These are not worked ...
            ("cam_sharpness", po::value<int>()->default_value(-1), "Camera's sharpness (int): min=0 max=255 step=1 default=128")
            ("cam_backlight_compensation", po::value<int>()->default_value(-1), "Camera's backlight_compensation (int): min=0 max=1 step=1 default=0")
            ("cam_white_balance_temperature", po::value<int>()->default_value(-1), "Camera's white_balance_temperature (int): min=2000 max=6500 step=1 default=4000")
            ;
            /*
            Not supported by opencv:
            ("cam_white_balance_temperature_auto", po::value<bool>()->default_value(true), "Camera's white_balance_temperature_auto (bool)   : default=1 value=0")
            power_line_frequency (menu)   : min=0 max=2 default=2 value=2
            exposure_auto_priority (bool)   : default=0 value=1

            Looks like unuseful:
                   pan_absolute (int)    : min=-36000 max=36000 step=3600 default=0 value=0
                  tilt_absolute (int)    : min=-36000 max=36000 step=3600 default=0 value=0
                 focus_absolute (int)    : min=0 max=250 step=5 default=0 value=0 flags=inactive
                  zoom_absolute (int)    : min=100 max=500 step=1 default=100 value=100
            */
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
        LOG_INFO("Video stream from file=" << src_file);
		src = PVideoStream(new FileVideoStream(src_file, 0, -1));
	} else {
        LOG_INFO("Video stream from camera 0");
        CameraParameters cp;
        cp.width = vm["cam_width"].as<int>();
        cp.height = vm["cam_height"].as<int>();
        cp.fps = vm["cam_fps"].as<int>();
        cp.brightness = vm["cam_brightness"].as<int>()/255.0;
        cp.contrast = vm["cam_contrast"].as<int>()/255.0;
        cp.saturation = vm["cam_saturation"].as<int>()/255.0;
        cp.sharpness = vm["cam_sharpness"].as<int>()/255.0;
        cp.gain = vm["cam_gain"].as<int>()/255.0;
        cp.temperature = vm["cam_white_balance_temperature"].as<int>();
        cp.backlight = vm["cam_backlight_compensation"].as<int>();
        cp.exposure = vm["cam_exposure_absolute"].as<int>();
        cp.auto_exposure = vm["cam_exposure_auto"].as<int>();
        cp.autofocus = vm["cam_autofocus"].as<bool>();
        std::string fourcc = vm["cam_fourcc"].as<std::string>();
        if(fourcc.length() == 4){
            cp.fourcc = cv::VideoWriter::fourcc(fourcc[0], fourcc[1], fourcc[2], fourcc[3]);
        }
        src = PVideoStream(new WebcamVideoStream(cp));
	}

	std::string res_path = vm.count("res_path") ? vm["res_path"].as<std::string>() : "./";
	LOG_INFO("Will load resource files from " << res_path);
	std::shared_ptr<DnnFaceRecognitionNet> rn(new DnnFaceRecognitionNet(res_path + "dlib_face_recognition_resnet_model_v1.dat",
			res_path + "shape_predictor_68_face_landmarks.dat"));
	PRecognitionManager rm(new RecognitionManager(rn));

	// Creating gRPC connection to Service Provider
	std::string sp_address = vm["sp_address"].as<std::string>();
	LOG_INFO("Service provider address is " << sp_address);

	// Auth token
	std::string access_key = vm.count("access_key") ? vm["access_key"].as<std::string>() : "";
	if (access_key.size() == 0) {
		LOG_WARN("No access key.");
	} else {
		LOG_INFO("access key is " << access_key);
	}

	std::string secret_key = vm.count("secret_key") ? vm["secret_key"].as<std::string>() : "";
	if (secret_key.size() == 0) {
		LOG_WARN("No secret key.");
	}

	// Creating video stream consumer
	fproc::PVideoStreamConsumer dst;
	if (vm["dst_display"].as<bool>()) {
		LOG_INFO("Will show video in a window");
		dst = PVideoStreamConsumer(new ImageWindow());
	} else {
		LOG_INFO("Using standard scene detector");
		SceneDetector* sd = new SceneDetector(PSceneDetectorListener(new FpcpConnector(sp_address, access_key, secret_key)), rm);
		if (!vm["scn_no_visual"].as<bool>()) {
			LOG_INFO("Will visualize scene detection");
			sd->setVisualizer(new SceneDetectorVisualizer(*sd));
		}
        HogParameters hp;
        hp.grayscale = vm["hog_grayscale"].as<bool>();
        hp.height = vm["hog_height"].as<int>();
        hp.width = vm["hog_width"].as<int>();
        sd->setHogParameters(hp);
		dst = PVideoStreamConsumer(sd);
	}

	fproc::VideoStreamCopier vsc(src, dst);
	vsc.process();

	return 0;
}
