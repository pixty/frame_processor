/*
 * face_detector.hpp
 *
 *  Created on: Jan 31, 2017
 *      Author: dmitry
 */

#ifndef SRC_FRAME_PROCESSING_FACE_DETECTOR_HPP_
#define SRC_FRAME_PROCESSING_FACE_DETECTOR_HPP_

#include "../model.hpp"

namespace fproc {

class FaceDetector: public ObjectDetector {
public:
	virtual FRList& detectRegions(PFrame pFrame);

private:
	FRList _frame_regions;
};

}

#endif /* SRC_FRAME_PROCESSING_FACE_DETECTOR_HPP_ */
