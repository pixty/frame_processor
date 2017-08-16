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

struct Face;

struct RecognitionManager {
	RecognitionManager(std::shared_ptr<DnnFaceRecognitionNet> rn): _rn(rn) {}

	// Returns list of faces found between provided frames
	PFrameFaceList recognize(PFrame& frame, PFrameRegList& frameRegs);

	// every face has no more than maxVectorsPerFace vectors. All faces should not exceed maxVectors
	void setLimits(int maxVectors, int maxVectorsPerFace) {
		max_vectors_ = maxVectors;
		max_vectors_per_face_ = maxVectorsPerFace;
	}
private:
	typedef std::list<Face*> face_ptrs;
	bool isTheFace(Face& face, PFrameRegion& reg, bool log = false);
	void sweep();

	// constrains
	int max_vectors_ = 50000;
	int max_vectors_per_face_ = 50;

	std::shared_ptr<DnnFaceRecognitionNet> _rn;
	face_ptrs faces_;
	int sweep_count_ = 0;
};
typedef std::shared_ptr<RecognitionManager> PRecognitionManager;

}


#endif /* SRC_RECOGNIZER_RECOGNITION_MANAGER_HPP_ */