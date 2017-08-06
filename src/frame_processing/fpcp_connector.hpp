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

namespace fproc {

class FpcpConnector: public SceneDetectorListener {

public:
	FpcpConnector(std::string sp_address, std::string akey, std::string skey): sp_address_(sp_address), access_key_(akey), secret_key_(skey) , scene_detector_(NULL) {
		reconnect();
	}
	void onStarted(VideoStreamConsumer& sceneDetector);
	void onStopped();
	// Unblocking call which sends a signal to scene_updater() to update the scene.
	void onFaces(const FrameFaceList& fcList);

private:
	// Them method is updating scenes in a separate thread.
	void scene_updater();

	void reconnect() {
		scp_ = fpcp::rpc::connectGRPC(sp_address_, access_key_, secret_key_);
	}

	std::string sp_address_;
	std::string access_key_;
	std::string secret_key_;
	fpcp::rpc::PSceneProcessor scp_;

	VideoStreamConsumer* scene_detector_;
	PThread thread_;
	boost::mutex lock_;
	boost::condition_variable cond_;
};

}


#endif /* SRC_FRAME_PROCESSING_FPCP_CONNECTOR_HPP_ */
