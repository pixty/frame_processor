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
#include "../config_params.hpp"

namespace fproc {

struct HogFaceDetector: public ObjectDetector {
	HogFaceDetector();
	const PFrameRegList & detectRegions(PFrame pFrame);
    void setParameters(const HogParameters &params){_params = params;}
private:
	dlib::frontal_face_detector _detector;
    HogParameters _params;
};

}

#endif /* SRC_FRAME_PROCESSING_OBJECT_DETECTORS_HPP_ */
