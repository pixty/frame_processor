/*
 * show_sream_detector.cpp
 *
 *  Created on: Jan 26, 2017
 *      Author: dmitry
 */

#include "show_stream_detector.hpp"

#include "../logger.hpp"

namespace fproc {

ShowStreamDetector::ShowStreamDetector(VideoStream& vstream): SceneDetector(vstream, nil_sc_detecor_listener)  {

}

ShowStreamDetector::~ShowStreamDetector() {
}

void ShowStreamDetector::doProcess(PFrame frame) {
	if (imgWindow.isClosed()) {
		LOG_INFO("Window is closed. Game over.");
		stop();
		return;
	}
	imgWindow.show(frame);
}

} /* namespace fproc */
