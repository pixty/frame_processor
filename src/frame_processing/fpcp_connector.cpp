/*
 * fpcp_connector.cpp
 *
 *  Created on: Aug 6, 2017
 *      Author: dmitry
 */

#include "fpcp_connector.hpp"
#include "../logger.hpp"

namespace fproc {

void FpcpConnector::onStarted(VideoStreamConsumer& sceneDetector) {
	scene_detector_ = &sceneDetector;
	LOG_INFO("FpcpConnector: got onStarted() signal from a scene detector, running scene updater thread.");
	thread_.reset(new boost::thread(&FpcpConnector::scene_updater, this));
};

void FpcpConnector::onStopped() {
	LOG_INFO("FpcpConnector: got onStopped() signal, wrapping up.");
};

void FpcpConnector::onFaces(const FrameFaceList& fcList) {
	fpcp::rpc::Scene s;
	try {
		scp_->onScene(s);
	} catch (fpcp::rpc::Error& e) {
		LOG_WARN("Got exception from FpcpClient: " << e);
		if (e == fpcp::rpc::Error::WRONG_CREDENTIALS) {
			LOG_ERROR("FpcpClient reports about wrong credentials, stopping processing :(.");
			scene_detector_->close();
		}
	}
}

void FpcpConnector::scene_updater() {
	LOG_INFO("FpcpConnector: entering scene_updater() flow.");
	LOG_INFO("FpcpConnector: exiting scene_updater()");
}

}

