/*
 * scene_detector.cpp
 *
 *  Created on: Jun 14, 2017
 *      Author: dmitry
 */

#include "object_detectors.hpp"
#include "../logger.hpp"
#include "helpers.hpp"

namespace fproc {

bool SceneDetector::consumeFrame(PFrame frame) {
	if (_state == ST_INIT) {
		_state = ST_STARTED;
		LOG_INFO("SceneDetector: just started");
		_listener->onStarted(*this);
		if (_sc_visualizer) _sc_visualizer->start();
	}

	if (_state != ST_STARTED) {
		LOG_ERROR(
				"SceneDetector: unexpected state in consumeFrame(), state=" << _state << " request to close it...");
		return false;
	}

	LOG_DEBUG("SceneDetector: before detecting face");
	const FRList& hog_result = _hog_detector.detectRegions(frame);
	LOG_DEBUG("SceneDetector: after detecting face");

	if (hog_result.size() > 0) {
		_obj_tracker.setRegion(hog_result.front());
	} else {
		_obj_tracker.startTracking();
	}

	if (_sc_visualizer) {
		PFrameRegion track_reg = _obj_tracker.track(frame);
		_sc_visualizer->onScene(frame, hog_result, track_reg);
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

void SceneDetectorVisualizer::onScene(PFrame frame, const FRList& hog_result, PFrameRegion track_reg) {
	if (_new_frame) {
		return;
	}

	MxGuard guard(_lock);
	_new_frame = true;
	_frame = frame;
	_hog_result = hog_result;
	_track_reg = track_reg;
	//_hog_result.insert(_hog_result.end(), hog_result.begin(), hog_result.end());
	_cond.notify_one();
}

void SceneDetectorVisualizer::start() {
	_thread.reset(new boost::thread(&SceneDetectorVisualizer::run, this));
}

void SceneDetectorVisualizer::run() {
	LOG_INFO("SceneDetectorVisualizer: starting thread");
	while (!isClosed()) {
		{
			MxGuard guard(_lock);
			while (!_new_frame && !isClosed()) {
				_cond.wait(guard);
			}

			if (isClosed()) {
				break;
			}

		}

		if (!_hog_result.size()) {
			_win.set_image(_frame->get_cv_image());
			_new_frame = false;
			continue;
		}

		cv::Mat im(_frame->get_mat());
		for (PFrameRegion pfr: _hog_result) {
			CvRect cvr = toCvRect(pfr->getRectangle());
			rounded_rectangle(im, cvr, cv::Scalar(58, 242, 252), 1, 16, 5);
		}

		if (_track_reg.get()) {
			CvRect cvr = toCvRect(_track_reg->getRectangle());
			rounded_rectangle(im, cvr, cv::Scalar(255, 100, 0), 1, 16, 5);
		}

		Frame::DlibBgrImg img(im);
		_win.set_image(img);
		_new_frame = false;
	}
	LOG_INFO("SceneDetectorVisualizer: stopping thread");
}

void SceneDetectorVisualizer::close() {
	LOG_INFO("SceneDetectorVisualizer: close() invoked");
	_win.close_window();
	MxGuard guard(_lock);
	_cond.notify_all();
}

}
