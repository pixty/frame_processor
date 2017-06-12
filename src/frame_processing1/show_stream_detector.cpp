/*
 * show_sream_detector.cpp
 *
 *  Created on: Jan 26, 2017
 *      Author: dmitry
 */

#include "../frame_processing1/show_stream_detector.hpp"

#include "../logger.hpp"

namespace fproc {

ShowStreamDetector::ShowStreamDetector(PVideoStream vstream, const ShowStreamDetectorCfg &cfg): 
	SceneDetector(std::move(vstream), 
		      PSceneDetectorListener(new SceneDetectorListener())),
	_faceDetector(cfg.faceLandmarksModelFilename())
{
}

ShowStreamDetector::~ShowStreamDetector() {
}

void ShowStreamDetector::doProcess(PFrame frame) {
	if (_imgWindow.isClosed()) {
		LOG_INFO("Window is closed. Game over.");
		stop();
		return;
	}
	_faceDetector.detectRegions(frame);
	_imgWindow.show(frame);
}

} /* namespace fproc */
