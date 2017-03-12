/*
 * face_detector.hpp
 *
 *  Created on: Jan 31, 2017
 *      Author: dmitry
 */

#ifndef SRC_FRAME_PROCESSING_FACE_DETECTOR_HPP_
#define SRC_FRAME_PROCESSING_FACE_DETECTOR_HPP_

#include "../model.hpp"
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>

namespace fproc {

class FaceDetector: public ObjectDetector {
public:
	FaceDetector(const std::string &faceLandmarksModelFilename);
	virtual FRList& detectRegions(PFrame pFrame);

private:
	void demo(cv::Mat &out, const int frame, std::vector<dlib::rectangle> faces);
	void demo1(cv::Mat &out, const int frame, std::vector<dlib::rectangle> faces);
	void effect(cv::Mat&out, const std::vector<dlib::rectangle> &faces, const int effect);

	FRList _frame_regions;
	dlib::frontal_face_detector _detector;
	dlib::shape_predictor _pose_model;
};

}

#endif /* SRC_FRAME_PROCESSING_FACE_DETECTOR_HPP_ */
