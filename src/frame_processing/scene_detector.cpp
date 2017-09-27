/*
 * scene_detector.cpp
 *
 *  Created on: Jun 14, 2017
 *      Author: dmitry
 */

#include "scene_detector.hpp"
#include "../logger.hpp"
#include "helpers.hpp"
#include "image_processing.hpp"

namespace fproc {

SceneDetector::SceneDetector(PSceneDetectorListener listener, PRecognitionManager recManager):
		_listener(std::move(listener)), _rec_manager(recManager), _state(ST_INIT), min_sharpness_(80.0) {
}

void SceneDetector::setHogParameters(const HogParameters &params){
    _hog_detector.setParameters(params);
}

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

	LOG_DEBUG("SceneDetector: *** before detecting faces ***");
	PFrameRegList hog_result = _hog_detector.detectRegions(frame);
	LOG_DEBUG("SceneDetector(): filtering faces - " << filterFrames(frame, hog_result) << " rejected due to sharpness less than " << min_sharpness_);
	LOG_DEBUG("SceneDetector:	" << hog_result.size() << " regions found, recognizing...");
	PFrameFaceList fflist(_rec_manager->recognize(frame, hog_result));
	LOG_DEBUG("SceneDetector:	" << fflist->size() << " faces recognized, forming scene...");
	PScene ps(new Scene(frame, fflist));
	if (_scene_state.onScene(ps)) {
		_listener->onScene(ps);
	}
	LOG_DEBUG("SceneDetector: --- after detecting face ---");

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

void SceneDetector::setMinSharpness(double sharpness) {
	LOG_INFO("SceneDetector(): Setting minimum sharpness to " << sharpness);
	min_sharpness_ = sharpness;
}

// The method filters the frames detected and every one which has sharpness less
// than minimum allowed (min_sharpness_) is dropped.
int SceneDetector::filterFrames(PFrame& frame, PFrameRegList& frames) {
	int deleted = 0;
	std::list<PFrameRegion>::iterator it = frames.begin();
	Size frmSz = frame->size();
	while (it != frames.end()) {
		PFrameRegion pfr = *it;
		const Rectangle& rect = pfr->getRectangle();
		if (!RectangleInFrame(rect, frmSz)) {
			it = frames.erase(it);
			deleted++;
			continue;
		}

		double shpns = sharpness(frame, pfr->getRectangle());
		if (shpns < min_sharpness_) {
			it = frames.erase(it);
			deleted++;
			LOG_INFO("SceneDetector(): Dropping frame due to its sharpness=" << shpns << " or wrong rectangle size.");
			continue;
		}

		pfr->set_sharpness(shpns);
		++it;
	}
	return deleted;
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

static int compare(SceneState::FaceSet& s1, SceneState::FaceSet& s2) {
	if (s1.size() != s2.size()) {
		return  s1.size() > s2.size() ? 1 : -1;
	}

	for (auto &e: s1) {
		if (s2.find(e) == s2.end()) {
			return 1;
		}
	}
	return 0;
}

static void to_map(const PFrameFaceList& fl, SceneState::FaceSet& s) {
	s.clear();
	for (auto &f: *fl) {
		s.insert(f);
	}
}

bool SceneState::onScene(PScene ps) {
	Timestamp now = ts_now();
	SceneState::FaceSet faceSet;
	to_map(ps->getFrameFaceList(), faceSet);
	if (_state == SST_TRANSITION) {
		int cmp = compare(faceSet, _lastReportedFaces);
		_lastReportedFaces = faceSet;

		if (_state_since + _transitionTimeoutMs < now || cmp > 0) {
			// Time is out, or somebody come to the scene
			LOG_DEBUG("SceneState switching to SST_OBSERVING by timeout.");
			_state_since = now;
			_state = SST_OBSERVING;
			if (compare(_facesOnScene, faceSet) != 0) {
				_scene_since = now;
				next_report_at_ = 0;
				LOG_INFO("SceneState: switching to SST_OBSERVING, but with a new person on the scene, or sombebody gone. Now " << faceSet.size() << " persons.");
			}
			_facesOnScene = faceSet;
			return shouldReport(now, ps);
		}

		if (cmp < 0) {
			// somebody else disappear
			_state_since += _transitionTimeoutMs/2;
			LOG_DEBUG("SceneState: prolonging SST_TRANSITION due to a person gone.");
		}
		return shouldReport(now, ps);
	}

	//SST_OBSERVING here
	int cmp = compare(faceSet, _facesOnScene);
	if (cmp > 0) {
		// A new person come
		LOG_INFO("SceneState: New person detected. Now " << faceSet.size() << " persons on the scene.");
		_state_since = now;
		_scene_since = now;
		next_report_at_ = 0;
		_facesOnScene = faceSet;
		return shouldReport(now, ps);
	}

	if (cmp < 0) {
		LOG_DEBUG("SceneState: Somebody disappeared, switching to SST_TRANSITION");
		_state = SST_TRANSITION;
		_state_since = now;
		_lastReportedFaces = faceSet;
	}
	return shouldReport(now, ps);
}

bool SceneState::shouldReport(Timestamp at, PScene ps) {
	if (at < next_report_at_) {
		return false;
	}

	ps->setId(std::to_string(_scene_since));
	ps->setSince(_scene_since);
	ps->setPersons((int)_facesOnScene.size());

	if (_facesOnScene.size() == 0) {
		next_report_at_ = at + 5000;
	} else if (at - _scene_since > 5000) {
		next_report_at_ = at + 5000;
	} else if (at - _scene_since > 3000) {
		next_report_at_ = at + 2000;
	} else if (at - _scene_since > 1000) {
		next_report_at_ = at + 1000;
	} else {
		next_report_at_ = at;
	}

	return true;
}

void SceneState::setTransitionTimeout(long tt_ms) {
	_transitionTimeoutMs = tt_ms;
}


}
