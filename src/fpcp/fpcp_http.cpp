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
		LOG_WARN("FprocEndHttp: already started, ignoring the call, but listener is changed");
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

void FprocEndHttp::sendScene(const fproc::Scene& scene) {

}

void FprocEndHttp::sendImage(const fproc::FrameRegion& frameReg) {

}

void FprocEndHttp::sendPerson(const fproc::Face& face) {

}

void FprocEndHttp::getF() {
	LOG_INFO("FprocEndHttp: entering getF");
	string url = _url + _id;
	while (_started) {
		auto r = cpr::Get(cpr::Url{url}, cpr::Parameters{{"timeout", std::to_string(_get_timeout)}});
		LOG_DEBUG("FprocEndHttp: GET response: " << r.status_code << " body=" << r.text);
		if (r.status_code == 0) {
			LOG_WARN("FprocEndHttp: GET - cannot connect to " << url);
			boost::this_thread::sleep(boost::posix_time::seconds(1));
			continue;
		}
		if (r.status_code == 200) {
			FPCPReq req;
			from_json(r.text, req);

		}
	}
	LOG_INFO("FprocEndHttp: exiting getF");
}


void FprocEndHttp::postResponse(FPCPResp& resp) {

//	auto r = cpr::Post(cpr::Url{"http://www.httpbin.org/post"},
//	                   cpr::Multipart{{"key", "large value"},
//	                                  {"name", cpr::Buffer{content, content + length, "filename.txt"}}});

}



}



