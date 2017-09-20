/*
 * scene_detector_fpcp_connector.hpp
 *
 *  Created on: Aug 6, 2017
 *      Author: dmitry
 */

#ifndef SRC_FRAME_PROCESSING_FPCP_CONNECTOR_HPP_
#define SRC_FRAME_PROCESSING_FPCP_CONNECTOR_HPP_

#include "scene_detector.hpp"
#include "../fpcp_grpc/fpcp_client.hpp"
#include "../config_params.hpp"

namespace fproc {

class FpcpConnector: public SceneDetectorListener {

public:
	FpcpConnector(const FpcpClientParameters& params): params_(params) , scene_detector_(NULL) {
		reconnect();
	}

	void setMaxScenesSize(int scenesSize) { max_scenes_size_ = scenesSize; }
	void onStarted(VideoStreamConsumer& sceneDetector);
	void onStopped();
	// Unblocking call which sends a signal to scene_updater() to update the scene.
	void onScene(PScene ps);

private:
	// Them method is updating scenes in a separate thread.
	void scene_updater();

	void reconnect() {
		scp_ = fpcp::rpc::connectGRPC(params_);
	}

	int max_scenes_size_ = 50;

	FpcpClientParameters params_;
	fpcp::rpc::PSceneProcessor scp_;
	std::list<PScene> scenes_;

	volatile bool started_;
	VideoStreamConsumer* scene_detector_;
	PThread thread_;
	boost::mutex lock_;
	boost::condition_variable cond_;
};

}


#endif /* SRC_FRAME_PROCESSING_FPCP_CONNECTOR_HPP_ */
