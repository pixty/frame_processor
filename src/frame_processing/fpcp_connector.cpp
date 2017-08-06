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
	started_ = true;
	thread_.reset(new boost::thread(&FpcpConnector::scene_updater, this));
};

void FpcpConnector::onStopped() {
	LOG_INFO("FpcpConnector: got onStopped() signal, wrapping up.");
	MxGuard guard(lock_);
	started_ = false;
	cond_.notify_all();
};

void FpcpConnector::onScene(PScene ps) {
	MxGuard guard(lock_);
	scenes_.push_back(ps);
	if (scenes_.size() > max_scenes_size_) {
		int new_size = max_scenes_size_ - max(1, max_scenes_size_/10);
		LOG_WARN("FpcpConnector: scenes_.size() is " << scenes_.size() << ", but max_scenes_size_=" << max_scenes_size_ << " cutting scenes_size to " << new_size);
		while (scenes_.size() > new_size) {
			scenes_.pop_front();
		}
	}
	cond_.notify_all();
}

void FpcpConnector::scene_updater() {
	LOG_INFO("FpcpConnector: entering scene_updater() flow.");
	while (started_) {
		PScene ps;
		{
			MxGuard guard(lock_);
			while (started_ && scenes_.size() == 0) {
				cond_.wait(guard);
			}

			if (!started_) {
				break;
			}
			ps = scenes_.front();
		}

		try {
			scp_->onScene(ps);
			{
				MxGuard guard(lock_);
				scenes_.pop_front();
			}
		} catch (fpcp::rpc::Error& e) {
			LOG_WARN("Got exception from FpcpClient: " << e << " scenes in queue - " << scenes_.size());
			if (e == fpcp::rpc::Error::WRONG_CREDENTIALS) {
				LOG_ERROR("FpcpClient reports about wrong credentials, stopping processing :(.");
				scene_detector_->close();
			}
		}
	}
	LOG_INFO("FpcpConnector: exiting scene_updater()");
}

}

