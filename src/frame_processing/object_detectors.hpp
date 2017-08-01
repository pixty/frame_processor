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
#include <opencv2/tracking.hpp>
#include "../recognizer/recognition_manager.hpp"

namespace fproc {

struct HaarFaceDetector: public ObjectDetector {
	HaarFaceDetector(const int minFaceSize, const int maxFaceSize);
	virtual const PFrameRegList& detectRegions(PFrame pFrame);

private:
	cv::CascadeClassifier _cvFaceDetector;
	cv::Size _minFaceSize;
	cv::Size _maxFaceSize;
};

struct HogFaceDetector: public ObjectDetector {
	HogFaceDetector();
	PFrameRegList& detectRegions(PFrame pFrame);
	PFrameRegList& detectRegions(PFrame pFrame, const std::vector<Rectangle>& suggested_rects);
private:
	dlib::frontal_face_detector _detector;
};

struct ObjectTracker {
	ObjectTracker();
	void setRegion(PFrameRegion region);
	bool startTracking();
	PFrameRegion track(PFrame pFrame);
private:
	PFrameRegion _region;
	cv::Ptr<cv::Tracker> _cvTracker;
};


}

#endif /* SRC_FRAME_PROCESSING_OBJECT_DETECTORS_HPP_ */
