/*
 * recognition_manager.cpp
 *
 *  Created on: Jul 31, 2017
 *      Author: dmitry
 */

#include "recognition_manager.hpp"
#include "../logger.hpp"

namespace fproc {

void RecognitionManager::recognize(PFrameRegList& frameRegs, FrameFaceList& result) {
	for (auto &pfr: frameRegs) {
		_rn->set_vector(pfr);
		Face* pf = NULL;
		for(auto &knwn_face: _faces) {
			Face& face = knwn_face.second;
			if (isTheFace(face, pfr)) {
				// Adding the face to the list of known faces...
				pf = &face;
				LOG_DEBUG("Found a match of the face with already known face id=" << pf->id);
				break;
			}
		}

		if (!pf) {
			for(auto &knwn_face: _faces) {
				Face& face = knwn_face.second;
				isTheFace(face, pfr, true);
			}
			// We found new face, adding it here
			FaceId fid = uuid();
			pf = &_faces[fid];
			LOG_INFO("New face detected, pf=" << pf);
			pf->id = fid;
			LOG_INFO("New face detected, id=" << pf->id);
		}

		addRegionToFace(*pf, pfr);
		result.push_back(FrameFace(pf->id, pfr));
	}
}

bool RecognitionManager::isTheFace(Face& face, PFrameRegion& pfr, bool log) {
	if (log) {
		LOG_INFO("Looking for region=" << pfr->getRectangle());
	}

	for (auto &reg: face.regions) {
		PFrameRegion& fr = reg.second;
		float d = distance(fr->v128d(), pfr->v128d());
		if (log) {
			LOG_INFO("\twith region=" << fr->getRectangle() << ", distance d=" << d);
		}
		if (d < 0.58) {
			return true;
		}
	}
	return false;
}

void RecognitionManager::addRegionToFace(Face& face, PFrameRegion& reg) {
	FrameId frid = reg->getFrame()->getId();
	face.regions[frid] = reg;
}

}

