/*
 * fpcp_scene_detector_listener.hpp
 *
 *  Created on: Apr 22, 2017
 *      Author: dmitry
 */

#ifndef SRC_FRAME_PROCESSING_FPCP_SCENE_DETECTOR_LISTENER_HPP_
#define SRC_FRAME_PROCESSING_FPCP_SCENE_DETECTOR_LISTENER_HPP_

#include "../fpcp/fpcp_http.hpp"
#include <boost/thread/thread.hpp>

namespace fproc {

	struct FPCPSceneDetectorListenerCfg {
		FPCPSceneDetectorListenerCfg(): fp_id(""), url(""), get_timeout(0){}
		std::string fp_id; // the frame processor id
		std::string url; // fpcp server url
		int get_timeout; // long polling in seconds
	};

	class FPCPSceneDetectorListener: public SceneDetectorListener, fpcp::SPRequestListener {
		typedef std::shared_ptr<boost::thread> PThread;
		typedef boost::unique_lock<boost::mutex> MxGuard;

	public:
		FPCPSceneDetectorListener(const FPCPSceneDetectorListenerCfg &cfg);
		~FPCPSceneDetectorListener() {}

		// SceneDetectorListener
		void onStarted();
		void onSceneChanged(const Scene& scene);
		void onSceneUpdated(const Scene& scene);
		void onStopped();

		// fpcp::SPRequestListener
		void onSceneRequest() {}
		void onImageRequest(fpcp::id reqId, fpcp::id imgId) {}
		void onPersonRequest(fpcp::id reqId, fpcp::id personId) {}

	private:
		void on_new_scene(const Scene& scene);
		void run();

		std::shared_ptr<fpcp::FprocEndHttp> _fpcp;
		PThread _thread;
		volatile bool _started;
		PScene _scene;
		boost::mutex _lock;
		boost::condition_variable _cond;
	};
}



#endif /* SRC_FRAME_PROCESSING_FPCP_SCENE_DETECTOR_LISTENER_HPP_ */
