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

class HaarFaceDetector: public ObjectDetector {
public:
	HaarFaceDetector(const int minFaceSize, const int maxFaceSize);
	HaarFaceDetector(const char * model, const int minFaceSize=0, const int maxFaceSize = 0);
	virtual FRList& detectRegions(PFrame pFrame);

private:
	FRList _frame_regions;
	dlib::frontal_face_detector _detector;
	dlib::shape_predictor _pose_model;
};

}



#endif /* SRC_FRAME_PROCESSING_OBJECT_DETECTORS_HPP_ */
