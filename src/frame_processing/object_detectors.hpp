/*
 * object_detectors.hpp
 *
 *  Created on: May 30, 2017
 *      Author: dmitry
 */

#ifndef SRC_FRAME_PROCESSING_OBJECT_DETECTORS_HPP_
#define SRC_FRAME_PROCESSING_OBJECT_DETECTORS_HPP_

#include "../model.hpp"
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>

namespace fproc {

struct HaarFaceDetector: public ObjectDetector {
	HaarFaceDetector(const int minFaceSize, const int maxFaceSize);
	virtual const FRList& detectRegions(PFrame pFrame);

private:
	cv::CascadeClassifier _cvFaceDetector;
	cv::Size _minFaceSize;
	cv::Size _maxFaceSize;
};

struct HogFaceDetector: public ObjectDetector {
	HogFaceDetector();
	const FRList& detectRegions(PFrame pFrame);
	const FRList& detectRegions(PFrame pFrame, const std::vector<Rectangle>& suggested_rects);
private:
	dlib::frontal_face_detector _detector;
};

struct SceneDetectorVisualizer;
typedef std::unique_ptr<SceneDetectorVisualizer> PSceneDetectorVisualizer;

struct SceneDetector: public VideoStreamConsumer {
	SceneDetector(PSceneDetectorListener listener):_listener(std::move(listener)), _state(ST_INIT) {};

	void setVisualizer(SceneDetectorVisualizer* sdv) { _sc_visualizer = PSceneDetectorVisualizer(sdv); };
	bool consumeFrame(PFrame frame);
	void close();
private:
	const static int ST_INIT = 0;
	const static int ST_STARTED = 1;
	const static int ST_STOPPED = 2;

	PSceneDetectorListener _listener;
	HogFaceDetector _hog_detector;
	int _state;
	PSceneDetectorVisualizer _sc_visualizer;
};

struct SceneDetectorVisualizer: public SceneDetectorListener {
	SceneDetectorVisualizer(SceneDetector& scn_detector): _scn_detector(scn_detector) {}
	virtual ~SceneDetectorVisualizer();

	void onScene(PFrame frame, const FRList& hog_result);
	bool isClosed() { return _win.is_closed(); }
	void close();
private:
	SceneDetector& _scn_detector;
	dlib::image_window _win;
};

}

#endif /* SRC_FRAME_PROCESSING_OBJECT_DETECTORS_HPP_ */
