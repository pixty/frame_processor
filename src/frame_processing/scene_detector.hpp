/*
 * scene_detector.hpp
 *
 *  Created on: Aug 1, 2017
 *      Author: dmitry
 */

#ifndef SRC_FRAME_PROCESSING_SCENE_DETECTOR_HPP_
#define SRC_FRAME_PROCESSING_SCENE_DETECTOR_HPP_

#include "object_detectors.hpp"

namespace fproc {

struct SceneDetectorVisualizer;
typedef std::unique_ptr<SceneDetectorVisualizer> PSceneDetectorVisualizer;

struct SceneState {
	typedef std::map<FaceId, PFace> FaceMap;

	SceneState();
	void onFaces(PFaceList pfl);
	void setTransitionTimeout(long tt_ms);

private:
	/*
	 * Scene state. in the SST_OBSERVING state the people can come or nobody there, if
	 * somebody goes from the scene then the scene state switches to SST_TRANSITION.
	 * This state adds an delaying element there. If the disappear is caused by an error,
	 * then scene state switches back to SST_OBSERVING as soon, as a person come to the
	 * scene again. If timeout happens scene state switches to SST_OBSERVING but reported
	 * state becomes equal to the list of people on the scene.
	 */
	const static int SST_OBSERVING = 0;
	const static int SST_TRANSITION = 1;

	long _transitionTimeoutMs;

	FaceMap _facesOnScene;
	FaceMap _lastReportedFaces;
	int _state;
	Timestamp _state_since;
	Timestamp _scene_since;
};

struct SceneDetector: public VideoStreamConsumer {
	SceneDetector(PSceneDetectorListener listener, PRecognitionManager recManager);

	void setVisualizer(SceneDetectorVisualizer* sdv) { _sc_visualizer = PSceneDetectorVisualizer(sdv); };
	bool consumeFrame(PFrame frame);
	void close();
private:
	// Scene detector starting state
	const static int ST_INIT = 0;
	const static int ST_STARTED = 1;
	const static int ST_STOPPED = 2;

	PSceneDetectorListener _listener;
	PRecognitionManager _rec_manager;
	HogFaceDetector _hog_detector;
	int _state;
	SceneState _scene_state;
	PSceneDetectorVisualizer _sc_visualizer;
};

struct SceneDetectorVisualizer: public SceneDetectorListener {
	SceneDetectorVisualizer(SceneDetector& scn_detector): _scn_detector(scn_detector), _new_frame(false) {}
	virtual ~SceneDetectorVisualizer();

	void onScene(PFrame frame, const PFrameRegList& hog_result);
	bool isClosed() { return _win.is_closed(); }
	void start();
	void close();
protected:
	void run();
private:
	SceneDetector& _scn_detector;
	dlib::image_window _win;

	PFrame _frame;
	PFrameRegList _hog_result;
	volatile bool _new_frame;
	PThread _thread;
	boost::mutex _lock;
	boost::condition_variable _cond;
};

}
#endif /* SRC_FRAME_PROCESSING_SCENE_DETECTOR_HPP_ */
