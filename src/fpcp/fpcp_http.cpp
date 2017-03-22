/*
 * fpcp_http.cpp
 *
 *  Created on: Mar 6, 2017
 *      Author: dmitry
 */

#include <cpr/cpr.h>
#include "fpcp_http.hpp"
#include "../logger.hpp"
#include <boost/bind.hpp>

namespace fpcp {

void FprocEndHttp::start(SPRequestListener* listener) {
	MxGuard guard(_lock);
	_listener = listener;
	if (_started) {
		LOG_WARN(
				"FprocEndHttp: already started, ignoring the call, but listener is changed");
		return;
	}

	LOG_INFO("FprocEndHttp: starting...");
	_thread.reset(new boost::thread(&FprocEndHttp::getF, this));
	_started = true;
}

void FprocEndHttp::stop() {
	MxGuard guard(_lock);
	if (!_started) {
		LOG_WARN("FprocEndHttp: already stopped, ignoring the call");
		return;
	}
	LOG_INFO("FprocEndHttp: stopping...");
	_started = false;
	_thread->interrupt();
	_thread.reset();
}

void FprocEndHttp::sendScene(id reqId, const fproc::Scene& scene) {
	FPCPResp resp;
	resp.reqId = reqId;
	resp.scene = const_cast<fproc::Scene*>(&scene);
	postResponse(resp);
}

void FprocEndHttp::sendImage(id reqId, const fproc::FrameRegion& frameReg) {
	FPCPResp resp;
	resp.reqId = reqId;
	resp.image = const_cast<fproc::FrameRegion*>(&frameReg);
	postResponse(resp);
}

void FprocEndHttp::sendPerson(id reqId, const fproc::Face& face) {
	FPCPResp resp;
	resp.reqId = reqId;
	resp.person = const_cast<fproc::Face*>(&face);
	postResponse(resp);
}

void FprocEndHttp::sendError(id reqId, error error) {
	FPCPResp resp;
	resp.reqId = reqId;
	postResponse(resp);
}

void FprocEndHttp::getF() {
	LOG_INFO("FprocEndHttp: entering getF");
	string url = _url + _id;
	while (_started) {
		auto r = cpr::Get(cpr::Url { url }, cpr::Parameters { { "timeout",
				std::to_string(_get_timeout) } });
		LOG_DEBUG(
				"FprocEndHttp: GET response: " << r.status_code << " body=" << r.text);
		if (r.status_code == 0) {
			LOG_WARN("FprocEndHttp: GET - cannot connect to " << url);
			boost::this_thread::sleep(boost::posix_time::seconds(1));
			continue;
		}
		if (r.status_code == 200) {
			FPCPReq req;
			from_json(r.text, req);

			if (req.scene) {
				_listener->onSceneRequest();
			}

			if (is_empty(req.imgId)) {
				_listener->onImageRequest(req.reqId, req.imgId);
			}

			if (is_empty(req.personId)) {
				_listener->onPersonRequest(req.reqId, req.personId);
			}
		}
	}
	LOG_INFO("FprocEndHttp: exiting getF");
}

void FprocEndHttp::postResponse(FPCPResp& resp) {
	LOG_DEBUG("FprocEndHttp: POST response");
	string url = _url + _id;
	cpr::Response r;
	if (resp.image) {
		cv::Mat &mat = resp.image->getFrame()->get_mat();
		uchar *ptr = mat.data;
		long length = mat.total() * mat.elemSize();
		r = cpr::Post(cpr::Url{url},
						   cpr::Multipart{{"resp", to_json(resp)},
										  {"image", cpr::Buffer{ptr, ptr + length, "image"}}});

	} else {
		string json = to_json(resp);
		LOG_DEBUG("FprocEndHttp: response=" << json);
		r = cpr::Post(cpr::Url{url},
						   cpr::Multipart{{"resp", json}});
	}
	LOG_DEBUG("FprocEndHttp: POST response: " << r.status_code << " body=" << r.text);
	if (r.status_code == 0) {
		LOG_WARN("FprocEndHttp: POST - cannot connect to " << url);
	}
}

}

