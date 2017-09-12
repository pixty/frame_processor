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
#include "../recognizer/recognition_manager.hpp"

namespace fproc {

struct HogFaceDetector: public ObjectDetector {
	HogFaceDetector();
	const PFrameRegList & detectRegions(PFrame pFrame);
private:
	dlib::frontal_face_detector _detector;
};

}

#endif /* SRC_FRAME_PROCESSING_OBJECT_DETECTORS_HPP_ */
