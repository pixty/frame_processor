/*
 * recognition_manager.hpp
 *
 *  Created on: Jul 31, 2017
 *      Author: dmitry
 */

#ifndef SRC_RECOGNIZER_RECOGNITION_MANAGER_HPP_
#define SRC_RECOGNIZER_RECOGNITION_MANAGER_HPP_

#include "dnn_face_recognition_model.hpp"

namespace fproc {

struct RecognitionManager {
	RecognitionManager(std::shared_ptr<DnnFaceRecognitionNet> rn): _rn(rn) {}

	// Returns list of faces found between provided frames
	PFaceList recognize(PFrameRegList& frameRegs);
private:
	bool isTheFace(PFace& face, PFrameRegion& reg, bool log = false);
	void addRegionToFace(PFace& face, PFrameRegion& reg);

	std::shared_ptr<DnnFaceRecognitionNet> _rn;
	std::map<FaceId, PFace> _faces;
};
typedef std::shared_ptr<RecognitionManager> PRecognitionManager;

}


#endif /* SRC_RECOGNIZER_RECOGNITION_MANAGER_HPP_ */
