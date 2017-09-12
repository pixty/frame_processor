/*
 * recognition_manager.cpp
 *
 *  Created on: Jul 31, 2017
 *      Author: dmitry
 */

#include "recognition_manager.hpp"
#include "../logger.hpp"

namespace fproc {

struct Face {
	FaceId id;
	list<PFrameRegion> regions;
};

PFrameFaceList RecognitionManager::recognize(const PFrame& frame, const PFrameRegList& frameRegs) {
	PFrameFaceList result(new FrameFaceList());
	for (auto &pfr: frameRegs) {
		_rn->set_vector(frame, pfr);
		Face* pf = NULL;
		for(face_ptrs::iterator knwn_face = faces_.begin(); knwn_face != faces_.end(); knwn_face++) {
			Face& face = **knwn_face;
			if (isTheFace(face, pfr)) {
				pf = &face;
				if (knwn_face != faces_.begin()) {
					faces_.erase(knwn_face);
					faces_.push_front(pf);
				}
				LOG_DEBUG("Found a match of the face with already known face id=" << pf->id);
				break;
			}
		}

		if (!pf) {
			// We found new face, adding it here
			FaceId fid = uuid();
			pf = new Face();
			faces_.push_front(pf);
			pf->id = fid;
			LOG_INFO("New face detected, id=" << pf->id);
		}

		pf->regions.push_back(pfr);
		result->push_back(FrameFace(pf->id, pfr));
	}
	sweep();
	return result;
}

bool RecognitionManager::isTheFace(const Face& face, const PFrameRegion& pfr, const bool log) const {
	if (log) {
		LOG_INFO("Looking for region=" << pfr->getRectangle());
	}

	for (auto &fr: face.regions) {
		float d = distance(*fr->v128d(), *pfr->v128d());
		if (log) {
			LOG_INFO("\twith region=" << fr->getRectangle() << ", distance d=" << d);
		}
		if (d < 0.59) {
			return true;
		}
	}
	return false;
}

void RecognitionManager::sweep() {
	if (sweep_count_++ < max_vectors_per_face_) {
		return;
	}
	sweep_count_ = 0;
	int count = 0;
	for(auto &face: faces_) {
		while (face->regions.size() > max_vectors_per_face_) {
			face->regions.pop_front();
		}
		count += face->regions.size();
	}

	while (count > max_vectors_) {
		Face* f = faces_.back();
		count -= f->regions.size();
		// oldest one
		faces_.pop_back();
	}
}

}
