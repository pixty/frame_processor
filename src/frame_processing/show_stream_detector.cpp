/*
 * show_sream_detector.cpp
 *
 *  Created on: Jan 26, 2017
 *      Author: dmitry
 */

#include "show_stream_detector.hpp"

#include "../logger.hpp"

namespace fproc {

ShowStreamDetector::ShowStreamDetector(std::string outFile, VideoStream& vstream): SceneDetector(vstream, nil_sc_detecor_listener)  {
	this->outFile = outFile;
}

ShowStreamDetector::~ShowStreamDetector() {
}

void ShowStreamDetector::doProcess(PFrame frame) {
	if (imgWindow.isClosed()) {
		LOG_INFO("Window is closed. Game over.");
		stop();
		return;
	}
	FRList &list = faceDetector.detectRegions(frame);
	std::ofstream faces_file(this->outFile);
	faces_file << list.size();
	faces_file.close();
	imgWindow.show(frame);
}

} /* namespace fproc */
