/*
 * scene_detector.cpp
 *
 *  Created on: Jun 14, 2017
 *      Author: dmitry
 */

#include "scene_detector.hpp"
#include "../logger.hpp"
#include "helpers.hpp"

namespace fproc {

SceneDetector::SceneDetector(PSceneDetectorListener listener, PRecognitionManager recManager):
		_listener(std::move(listener)), _rec_manager(recManager), _state(ST_INIT) {

};

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
	PFrameRegList& hog_result = _hog_detector.detectRegions(frame);
	PFaceList pfl = _rec_manager->recognize(hog_result);
	_scene_state.onFaces(pfl);
	LOG_DEBUG("SceneDetector: after detecting face");

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

// ========================= SceneDetectorVisualizer =========================

SceneDetectorVisualizer::~SceneDetectorVisualizer() {
	if (!_win.is_closed()) {
		_win.close_window();
	}
}

void SceneDetectorVisualizer::onScene(PFrame frame, const PFrameRegList& hog_result) {
	if (_new_frame) {
		return;
	}

	MxGuard guard(_lock);
	_new_frame = true;
	_frame = frame;
	_hog_result = hog_result;
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
			_win.set_image(_frame->get_bgr_image());
			_new_frame = false;
			continue;
		}

		cv::Mat im(_frame->get_mat());
		for (PFrameRegion pfr: _hog_result) {
			CvRect cvr = toCvRect(pfr->getRectangle());
			rounded_rectangle(im, cvr, cv::Scalar(58, 242, 252), 1, 16, 5);
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

// ================================ SceneState ===============================
SceneState::SceneState(): _transitionTimeoutMs(4000), _state(SST_OBSERVING) {
	_state_since = ts_now();
	_scene_since = _state_since;
}

static int compare(SceneState::FaceMap& m1, SceneState::FaceMap& m2) {
	if (m1.size() != m2.size()) {
		return  m1.size() > m2.size() ? 1 : -1;
	}

	for (auto &e: m1) {
		if (m2.find(e.first) == m2.end()) {
			return 1;
		}
	}
	return 0;
}

static void to_map(PFaceList pfl, SceneState::FaceMap& m) {
	m.clear();
	for (auto &pf: pfl) {
		m[pf->getId()] = pf;
	}
}

void SceneState::onFaces(PFaceList pfl) {
	Timestamp now = ts_now();
	SceneState::FaceMap faceMap;
	to_map(pfl, faceMap);
	if (_state == SST_TRANSITION) {
		int cmp = compare(faceMap, _lastReportedFaces);
		_lastReportedFaces = faceMap;

		if (_state_since + _transitionTimeoutMs < now || cmp > 0) {
			// Time is out, or somebody come to the scene
			LOG_DEBUG("SceneState switching to SST_OBSERVING by timeout.");
			_state_since = now;
			_state = SST_OBSERVING;
			if (compare(_facesOnScene, faceMap) != 0) {
				_scene_since = now;
				LOG_INFO("SceneState: switching to SST_OBSERVING, but with a new person on the scene, or sombebody gone. Now " << faceMap.size() << " persons.");
			}
			_facesOnScene = faceMap;
			return;
		}

		if (cmp < 0) {
			// somebody else disappear
			_state_since += _transitionTimeoutMs/2;
			LOG_DEBUG("SceneState: prolonging SST_TRANSITION due to a person gone.");
		}
		return;
	}

	//SST_OBSERVING here
	int cmp = compare(faceMap, _facesOnScene);
	if (cmp > 0) {
		// A new person come
		LOG_INFO("SceneState: New person detected. Now " << faceMap.size() << " persons on the scene.");
		_state_since = now;
		_scene_since = now;
		_facesOnScene = faceMap;
		return;
	}

	if (cmp < 0) {
		LOG_DEBUG("SceneState: Somebody disappeared, switching to SST_TRANSITION");
		_state = SST_TRANSITION;
		_state_since = now;
		_lastReportedFaces = faceMap;
	}
}

void SceneState::setTransitionTimeout(long tt_ms) {
	_transitionTimeoutMs = tt_ms;
}


}
