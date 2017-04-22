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

#include "model.hpp"
#include "frame_processing/scene_jsonify.hpp"
#include "config/app_config.hpp"

#include "fpcp/fpcp_http.hpp"
#include <unistd.h>

namespace po = boost::program_options;

int main(int argc, char** argv) {
//	debug_enabled(true);
//	fpcp::FprocEndHttp end("1234", "http://localhost:5555/fpcp/");
//	end.withGetTimeout(1);
//	end.start(NULL);
//
//	fproc::Face face("1234", fproc::ts_now());
//	end.sendPerson("req1234", face);
//
//	usleep(5*1000000);
//	end.stop();
//	return 1;

	po::options_description desc("Allowed options");
	desc.add_options()("help,h", "print usage message")
			("gencfg,g", po::value<std::string>(), "generate config samples, see \"arg*.json\" files")
			("cfg,c", po::value<std::string>(), "use \"arg\" config file")
			("debug", po::value<bool>()->default_value(false), "enable debug log level");

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

	if (vm.count("gencfg") || vm.count("-g")) {
		const char *oname = vm.count("gencfg") ? "gencfg" : "-g";
		std::string prefix = vm[oname].as<std::string>();
		fproc::DefaultCfgs::create(prefix);
		return 0;
	}

	fproc::AppConfig appConfig;
	if (vm.count("cfg") || vm.count("-c")) {
		const char *oname = vm.count("cfg") ? "cfg" : "-c";
		std::string cfgFilename = vm[oname].as<std::string>();
		LOG_INFO("Use " << cfgFilename << " configuration file");
		appConfig.fromJson(cfgFilename);
	} else {
		LOG_INFO("There is no config file specified. The default configuration is used.");
	}

	LOG_INFO("Actual configuration:\n" << appConfig);
	appConfig.createSceneDetector()->process();
	return 0;
}
