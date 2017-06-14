/*
 * scene_detector.cpp
 *
 *  Created on: Jun 14, 2017
 *      Author: dmitry
 */

#include "object_detectors.hpp"
#include "../logger.hpp"

namespace fproc {

bool SceneDetector::consumeFrame(PFrame frame) {
	if (_state == ST_INIT) {
		_state = ST_STARTED;
		LOG_INFO("SceneDetector: just started");
		_listener->onStarted(*this);
	}

	if (_state != ST_STARTED) {
		LOG_ERROR(
				"SceneDetector: unexpected state in consumeFrame(), state=" << _state << " request to close it...");
		return false;
	}

	const FRList& hog_result = _hog_detector.detectRegions(frame);
	if (_sc_visualizer) {
		_sc_visualizer->onScene(frame, hog_result);
		return !_sc_visualizer->isClosed();
	}
	return true;
}

void SceneDetector::close() {
	if (_state != ST_STARTED) {
		LOG_WARN("SceneDetector: close() called, but the state=" << _state);
		return;
	}
	if (_sc_visualizer) _sc_visualizer->close();
	LOG_INFO("SceneDetector: closing...");
	_state = ST_STOPPED;
}

SceneDetectorVisualizer::~SceneDetectorVisualizer() {
	if (!_win.is_closed()) {
		_win.close_window();
	}
}

void SceneDetectorVisualizer::onScene(PFrame frame, const FRList& hog_result) {
	_win.set_image(frame->get_cv_image());
}

void SceneDetectorVisualizer::close() {
	_win.close_window();
}

}
