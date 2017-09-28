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

/**
 * The recognition manager does some facial recognition job to compare new faces with already known ones.
 *
 * Phantom - is a frame, which was not matched with any face, but its region appeared in a "proximity" -
 * geometrically intersects, covers or be covered by a face region, which shot was taken recently (several
 * humdreds ms before) phantom_ttl_ param.
 */
struct RecognitionManager {
	enum FaceCmpRes {
		YES = 0,
		NO,
		AMBIGUOUS
	};

	static const std::string CMP_RES_STRING[];

	RecognitionManager(std::shared_ptr<DnnFaceRecognitionNet> rn): _rn(rn) {}

	// Returns list of faces found between provided frames
	PFrameFaceList recognize(const PFrame& frame, const PFrameRegList& frameRegs);

	// every face has no more than maxVectorsPerFace vectors. All faces should not exceed maxVectors
	void setLimits(int maxVectors, int maxVectorsPerFace) {
		max_vectors_ = maxVectors;
		max_vectors_per_face_ = maxVectorsPerFace;
	}
private:
	typedef std::list<Face*> face_ptrs;

	float getSamenessDistance(Timestamp diff) const;
	bool isAPhantom(const PFrameRegion& pfr1, const PFrameRegion& pfr2) const;
	FaceCmpRes isTheFace(const Face& face, const PFrameRegion& reg, const bool log = false) const;
	void sweep();

	// constrains
	int max_vectors_ = 50000;
	int max_vectors_per_face_ = 50;
	// the minimum fraction of vectors (the fraction is in between [0..1]) that have to be positively
	// compared to get he positive answer about the face smeness.
	float sameness_ = 0.3;

	// the fraction of overlapping 2 rectangles areas for declaring an ambiguity in the comparison, the
	// value shoule be in [0..1]
	float min_overlap_ = 0.7;

	// the face recognition maximal distance
	float same_face_dist_ = 0.6;

	// phatom ttl - defines how long we can consider the images relevant
	// 1.5 seconds by default
	Timestamp phantom_ttl_ = 1500;

	std::shared_ptr<DnnFaceRecognitionNet> _rn;
	// The faces we already know
	face_ptrs faces_;
	// List of phantom shots
	PFrameRegList phantoms_;
	int sweep_count_ = 0;
};
typedef std::shared_ptr<RecognitionManager> PRecognitionManager;

}

#endif /* SRC_RECOGNIZER_RECOGNITION_MANAGER_HPP_ */
