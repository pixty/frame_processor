/*
 * fpcp_scene_detector_listener.cpp
 *
 *  Created on: Apr 22, 2017
 *      Author: dmitry
 */

#include "fpcp_scene_detector_listener.hpp"

#include <boost/exception/diagnostic_information.hpp>

namespace fproc {

FPCPSceneDetectorListener::FPCPSceneDetectorListener(
		const FPCPSceneDetectorListenerCfg &cfg) {
	LOG_INFO("FPCPSceneDetectorListener: constructing...");
	_fpcp.reset(new fpcp::FprocEndHttp(cfg.fp_id, cfg.url));
	_fpcp->withGetTimeout(cfg.get_timeout);
	_scene->since(ts_now());

	MxGuard guard(_lock);
	_started = true;
	_thread.reset(new boost::thread(&FPCPSceneDetectorListener::run, this));
	_fpcp->start(this);
}

void FPCPSceneDetectorListener::onSceneChanged(const Scene& scene) {
	on_new_scene(scene);
}

void FPCPSceneDetectorListener::onSceneUpdated(const Scene& scene) {
	on_new_scene(scene);
}

void FPCPSceneDetectorListener::on_new_scene(const Scene& scene) {
	MxGuard guard(_lock);
	if (!_started) {
		return;
	}
	_scene.reset(new Scene(const_cast<Scene&>(scene)));
	_cond.notify_one();
}

void FPCPSceneDetectorListener::run() {
	LOG_INFO("FPCPSceneDetectorListener: started thread.");
	while (_started) {
		PScene s;
		{
			MxGuard guard(_lock);
			if (!_scene.get()) {
				_cond.wait(guard);
			}
			if (!_scene.get() || !_started) {
				continue;
			}
			s = _scene;
			_scene.reset();
		}
		LOG_DEBUG("Sending sId=" << s->frame()->getFrame()->getId() << ", ts=" << s->since());
		fpcp::FPCPResp resp;
		resp.scene = s.get();
		if (s.get() && s.get()->frame().get()) {
			resp.image = s.get()->frame().get();
		}
		try {
			_fpcp->postResponse(resp);
		} catch (...) {
			LOG_ERROR("FPCPSceneDetectorListener: exception in POST: " << boost::current_exception_diagnostic_information());
		}
	}
	_fpcp->stop();
	LOG_INFO("FPCPSceneDetectorListener: leaving thread");
}

void FPCPSceneDetectorListener::onStopped() {
	LOG_INFO("FPCPSceneDetectorListener: onStopped() invoked.");
	MxGuard guard(_lock);
	if (!_started) {
		return;
	}
	_started = false;
	_cond.notify_all();
}

}

