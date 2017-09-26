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

PFrameFaceList RecognitionManager::recognize(const PFrame& frame, const PFrameRegList& frameRegs) {
	PFrameFaceList result(new FrameFaceList());
	LOG_DEBUG("" << faces_.size() << " already known faces");
	for (auto &pfr: frameRegs) {
		std::string fn = "/Users/dmitry/tmp/test_pics/" + std::to_string(frame->getId()) + ".jpeg";
		compress_frame_region_to_file(frame, pfr->getRectangle(), ImageSize::Code::ORIGINAL, 50, fn, CompType::JPEG, 95);
		LOG_INFO("!!!!!!!!!!!!!   Sharpness " << sharpness(frame, pfr->getRectangle()));

		_rn->set_vector(frame, pfr);
		Face* pf = NULL;
		for(face_ptrs::iterator knwn_face = faces_.begin(); knwn_face != faces_.end(); knwn_face++) {
			Face& face = **knwn_face;
			if (isTheFace(face, pfr, true)) {
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

// The function returns the maximum distance threshold depending on the time difference the images were taken.
static float getSamenessDistance(Timestamp diff) {
	diff = std::abs(diff);
	if (diff < 1000) {
		return 0.49;
	};
	if (diff < 5000) {
		return 0.5;
	}
	if (diff < 24*3600*1000) {
		return 0.53;
	}
	if (diff < 7*24*3600*1000) {
		return 0.55;
	}
	return 0.6;
}

bool RecognitionManager::isTheFace(const Face& face, const PFrameRegion& pfr, const bool log) const {
	if (log) {
		LOG_INFO("Looking for region=" << pfr->getRectangle());
	}

	int positive = 0;
	float avgDist = 0.0;
	for (auto &fr: face.regions) {
		float d = distance(*fr->v128d(), *pfr->v128d());
		float maxDistance = getSamenessDistance(pfr->getTimestamp() - fr->getTimestamp());
		if (log) {
			//LOG_INFO("\twith region=" << fr->getRectangle() << ", distance d=" << d << ", maxDist=" << maxDistance);
		}
		if (d < maxDistance) {
			positive++;
		}
		avgDist += d;
	}

	int total = face.regions.size();
	avgDist /= total;
	bool result = positive >= sameness_*total;
	LOG_INFO("result=" << result << " threashold=" << sameness_*total);
	LOG_INFO("Positives/total=" << positive << "/" << face.regions.size() << " avgDist=" << avgDist);
	return result;
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
