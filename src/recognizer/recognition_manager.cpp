/*
 * recognition_manager.cpp
 *
 *  Created on: Jul 31, 2017
 *      Author: dmitry
 */

#include "recognition_manager.hpp"
#include "../logger.hpp"

// the test purposes only
#include "../frame_processing/image_processing.hpp"

namespace fproc {

struct Face {
	FaceId id;
	list<PFrameRegion> regions;
};

const std::string RecognitionManager::CMP_RES_STRING[] = {"YES", "NO", "AMBIGUOUS"};
const std::string& toString(RecognitionManager::FaceCmpRes res) {
	return RecognitionManager::CMP_RES_STRING[res];
}

PFrameFaceList RecognitionManager::recognize(const PFrame& frame, const PFrameRegList& frameRegs) {
	PFrameFaceList result(new FrameFaceList());
	for (auto &pfr: frameRegs) {
		_rn->set_vector(frame, pfr);
		Face* pf = NULL;
		for(face_ptrs::iterator knwn_face = faces_.begin(); knwn_face != faces_.end(); knwn_face++) {
			Face& face = **knwn_face;
			RecognitionManager::FaceCmpRes cmp_res = isTheFace(face, pfr);
			if (cmp_res == AMBIGUOUS) {
				// ok keeps it in a phantoms list
				phantoms_.push_back(pfr);
				goto skipFrame;
			}

			if (cmp_res == YES) {
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
			// search between phantoms first
			for (auto &ph: phantoms_) {
				if (isAPhantom(pfr, ph)) {
					phantoms_.push_front(pfr);
					LOG_INFO("RecognitionManager: frame looks like a phantom comparing to other phantoms, skip it.");
					goto skipFrame;
				}
			}

			// We found new face, adding it here
			FaceId fid = uuid();
			pf = new Face();
			faces_.push_front(pf);
			pf->id = fid;
			LOG_INFO("New face detected, id=" << pf->id << ", total persons already known for now is " << faces_.size());
		}

		pf->regions.push_back(pfr);
		result->push_back(FrameFace(pf->id, pfr));
		skipFrame:;
	}
	sweep();
	return result;
}

// The function returns the maximum distance threshold depending on the time difference the images were taken.
float RecognitionManager::getSamenessDistance(Timestamp diff) const {
	diff = std::abs(diff);
	if (diff < 10000) {
		// within several seconds...
		return same_face_dist_ - 0.05;
	}
	return same_face_dist_;
}

// tests that pfr1 is a phantom or pfr2:
// - pfr1 timestamp should be very close to pfr2 AND
// - pfr1 is inside of pfr2 OR
// - pfr1 covers pfr2 OR
// - intersection area of pfr1 and pfr2 is greater than min_overlap_*pfr1.area()
bool RecognitionManager::isAPhantom(const PFrameRegion& pfr1, const PFrameRegion& pfr2) const {
	if (std::abs(pfr1->getTimestamp() - pfr2->getTimestamp()) > phantom_ttl_) {
		return false;
	}

	const Rectangle& r1 = pfr1->getRectangle();
	const Rectangle& r2 = pfr2->getRectangle();

	Rectangle ri = r1.intersect(r2);
	float a = 0, a1 = r1.area(), ai = ri.area();
	if (ai > 0.0) {
		a = a1/ai;
	}

	return firstRectInsideSecond(r2, r1) || a >min_overlap_;
}

RecognitionManager::FaceCmpRes RecognitionManager::isTheFace(const Face& face, const PFrameRegion& pfr, const bool log) const {
	// the flag shows that pfr looks like a phantom to any of the face pics
	bool phantom = false;
	int positive = 0;
	float avgDist = 0.0;

	for (auto &fr: face.regions) {
		float d = distance(*fr->v128d(), *pfr->v128d());
		Timestamp diff = pfr->getTimestamp() - fr->getTimestamp();
		float maxDistance = getSamenessDistance(diff);

		if (d < maxDistance) {
			positive++;
		}
		if (!phantom) {
			phantom = isAPhantom(pfr, fr);
		}

		avgDist += d;
	}

	int total = face.regions.size();
	avgDist /= total;
	RecognitionManager::FaceCmpRes result = positive > sameness_*total ? YES : NO;
	if (log && result == NO) {
		LOG_INFO("RecognitionManager: result=" << toString(result) << " threshold=" << sameness_*total);
		LOG_INFO("RecognitionManager: Positives/total=" << positive << "/" << face.regions.size() << " avgDist=" << avgDist << " sharpness=" << pfr->get_sharpness());
	}

	if (result == NO && phantom) {
		result = AMBIGUOUS;
		LOG_WARN("RecognitionManager::isTheFace(): AMBIGUOUS Positive/total=" << positive << "/" << face.regions.size() << " avgDist=" << avgDist << " the frame sharpness=" << pfr->get_sharpness());
	}

	return result;
}

void RecognitionManager::sweep() {
	// Cleaning up vectors per face
	int count = 0;
	for(auto &face: faces_) {
		while (face->regions.size() > max_vectors_per_face_) {
			face->regions.pop_front();
		}
		count += face->regions.size();
	}

	// Cleaning up phantoms
	Timestamp now = ts_now();
	while (phantoms_.size() > 0 && std::abs(now - phantoms_.back()->getTimestamp()) > phantom_ttl_) {
		phantoms_.pop_back();
	}

	if (sweep_count_++ < max_vectors_per_face_) {
		return;
	}
	sweep_count_ = 0;
	while (count > max_vectors_) {
		Face* f = faces_.back();
		count -= f->regions.size();
		// oldest one
		faces_.pop_back();
	}
}

}
