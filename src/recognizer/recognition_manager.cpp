/*
 * recognition_manager.cpp
 *
 *  Created on: Jul 31, 2017
 *      Author: dmitry
 */

#include "recognition_manager.hpp"
#include "../logger.hpp"

namespace fproc {

PFaceList RecognitionManager::recognize(PFrameRegList& frameRegs) {
	PFaceList result;
	for (auto &pfr: frameRegs) {
		_rn->set_vector(pfr);
		PFace pf;
		for(auto &knwn_face: _faces) {
			PFace& face = knwn_face.second;
			if (isTheFace(face, pfr)) {
				// Adding the face to the list of known faces...
				pf = face;
				LOG_DEBUG("Found a match of the face with already known face id=" << pf->getId());
				break;
			}
		}

		if (!pf.get()) {
			for(auto &knwn_face: _faces) {
				PFace& face = knwn_face.second;
				isTheFace(face, pfr, true);
			}
			// We found new face, adding it here
			pf = PFace(new Face(uuid()));
			_faces[pf->getId()] = pf;
			LOG_INFO("New face detected, id=" << pf->getId());
		}

		addRegionToFace(pf, pfr);
		result.push_back(pf);
	}
	return result;
}

bool RecognitionManager::isTheFace(PFace& face, PFrameRegion& pfr, bool log) {
	if (log) {
		LOG_INFO("Looking for region=" << pfr->getRectangle());
	}
	for (auto &reg: face->regions) {
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

void RecognitionManager::addRegionToFace(PFace& face, PFrameRegion& reg) {
	FrameId frid = reg->getFrame()->getId();
	face->regions[frid] = reg;
}

}

