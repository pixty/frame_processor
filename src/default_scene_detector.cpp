/*
 * default_scene_detector.cpp
 *
 *  Created on: Jan 24, 2017
 *      Author: dmitry
 */

#include <exception>
#include "model.hpp"
#include <boost/thread/locks.hpp>
#include "logger.hpp"

namespace fp_test {

static NilSceneDetectorListener nil_sc_detecor_listener;

SceneDetector::SceneDetector(VideoStream& vstream, SceneDetectorListener& listener) {
	_vstream = vstream;
	_listener = listener;
	_started = false;
}

void SceneDetector::process() {
	{
		boost::lock_guard guard(_lock);
		_started = true;
	}

	LOG_INFO("SceneDetector: Entering processing.");
	try {
		while (_started) {
			PFrame frame = _vstream.captureFrame();
			if (frame.get() == nullptr) {
				LOG_INFO("SceneDetector: got null frame. Stop processing");
				stop();
				break;
			}

			doProcess(frame);
		}
	} catch (std::exception &e) {
		LOG_ERROR("SceneDetector: Oops an exception happens: " << e);
		stop(); // just in case
	}
	LOG_INFO("SceneDetector: Exit processing.");
}

void SceneDetector::stop() {
	boost::lock_guard guard(_lock);
	if (!_started) {
		return;
	}
	LOG_INFO("SceneDetector: stop()");
	_started = false;
	_listener.onStopped();
}

DefaultSceneDetector::DefaultSceneDetector(VideoStream& vstream): SceneDetector(vstream, nil_sc_detecor_listener) {
}


DefaultSceneDetector::DefaultSceneDetector(VideoStream& vstream, SceneDetectorListener& listener): SceneDetector(vstream, listener) {

}

}

