#include "../frame_processing1/naive_scene_detector.hpp"

#include "../frame_processing1/grayscaler.hpp"
#include "../frame_processing1/haar_face_detector.hpp"
#include "../frame_processing1/hog_face_detector.hpp"
#include "grayscaler.hpp"
#include "../logger.hpp"

namespace fproc {

NaiveSceneDetector::NaiveSceneDetector(PVideoStream vstream,
		PSceneDetectorListener listener, PDebugger debugger,
		const NaiveSceneDetectorParameters &parameters) :
		SceneDetector(std::move(vstream), std::move(listener)), _debugger(
				std::move(debugger)), _box_overlap(parameters.centroidThresh(),
				parameters.areaThresh()), _maxFaces(parameters.maxFaces()), _multiTracker(
				parameters.maxFramesToLooseTrack(), _box_overlap), _foreground_det(
				new HaarFaceDetector(parameters.minFaceSize(),
						parameters.maxFaceSize())), _face_det(
				new HogFaceDetector()) {
}

NaiveSceneDetector::~NaiveSceneDetector() {
}

void NaiveSceneDetector::doProcess(PFrame frame) {
	(*_debugger)();
	// 1. transform the frame to a grayscale image
	Grayscaler::transform(frame->get_mat(), &_grayedFrame);

	// 2. run the trackers
	_multiTracker.track(_grayedFrame);

	// 3. run the detectors
	CvRois detectedFaces;
	detectFaces(&detectedFaces);

	// 4. merge the detections and the trackers
	FaceRegionsList detectedAndNotTracked;
	FaceRegionsList detectedAndTracked;
	FaceRegionsList trackedAtTheMoment;
	_multiTracker.faceRegions(&trackedAtTheMoment);
	parseDetectedFaces(detectedFaces, trackedAtTheMoment,
			&detectedAndNotTracked, &detectedAndTracked);

	// 5. start new trackers if some new faces are detected
	FaceRegionsList detectedAndStarted;
	FaceRegionsList detectedAndNotStarted;
	_multiTracker.start(_grayedFrame, detectedAndNotTracked,
			&detectedAndStarted, &detectedAndNotStarted);

	// 6. correct trackers with the best knowledge about the faces
	// Disclaimer: Unfortunately the opencv trackers can not be corrected
	// so the trackers are restarted if the difference of the regions is huge
	FaceIdsList lostFaces;
	_multiTracker.update(_grayedFrame, detectedAndTracked, &lostFaces);

	// 7. push the results to the scene
	FaceRegionsList allTracked;
	_multiTracker.faceRegions(&allTracked);
	updateScene(frame, allTracked, detectedAndStarted, detectedAndNotStarted,
			detectedAndTracked, lostFaces);
	// 8. debug
	(*_debugger)(*this, frame, allTracked, detectedAndStarted,
			detectedAndNotStarted, detectedAndTracked, lostFaces);
}

void NaiveSceneDetector::parseDetectedFaces(const CvRois &detected,
		const FaceRegionsList &tracked, FaceRegionsList *detectedAndNotTracked,
		FaceRegionsList *detectedAndTracked) {
	// find new objects in the final_dets which are not in the multi tracker
	CvRois detectedAndNotTrackedRois;
	_box_overlap.separate(detected, tracked, &detectedAndNotTrackedRois,
			detectedAndTracked);
	createFaceRegions(detectedAndNotTrackedRois, detectedAndNotTracked);
}

void NaiveSceneDetector::createFaceRegions(const CvRois &src,
		FaceRegionsList *dest) {
	dest->clear();
	for (const CvRoi roi : src) {
		const FaceId faceId = _uuid_generator.get();
		dest->push_back(FaceRegion(faceId, roi));
	}
}

void NaiveSceneDetector::detectFaces(CvRois *detectedFaces) {
	// detect foreground objects
	CvRois candidates;
	_foreground_det->detect(_grayedFrame, &candidates);
	// detect faces
	// Let's increase the rectangles by 10%
	//  due to slightly different approaches
	//  between the first one detector and the hog detector
	for (int i = 0; i < candidates.size(); i++) {
		CvRoi roi = candidates[i];
		double k = (min(roi.width, roi.height) * 10.0) / 100.0;
		CvRoi incRoi = CvRoi(roi.x - k / 2.0, roi.y - k / 2.0, roi.width + k,
				roi.height + k);
		candidates[i] = incRoi;
	}
	_face_det->detect(DlibYImg(_grayedFrame), candidates, detectedFaces);
}

void normalizeScene(Scene &scene, const PFrame &frame) {
	PFaceList& lst = scene.getFaces();
	PFaceList newLst;
	for (PFaceList::iterator it=lst.begin(); it != lst.end(); ++it) {
		PFace face = *it;
		const PFrameRegList& regs = face->getImages();
		const FrameRegion &fr = regs.back();
		PFrameRegList fr_list;
		fr_list.push_back(FrameRegion(frame, fr.getRectangle()));
		PFace pf(new Face(face->getId(), fr_list, face->firstTimeCatched(), face->lostTime()));
		newLst.push_back(pf);
	}
	scene.setFaces(newLst);
}

void NaiveSceneDetector::updateScene(const PFrame &frame,
		const FaceRegionsList &tracked,
		const FaceRegionsList &detectedAndStarted,
		const FaceRegionsList &detectedAndNotStarted,
		const FaceRegionsList &detectedAndTracked,
		const FaceIdsList &lostFaces) {

	if (detectedAndNotStarted.size() > 0) {
		/* I have no idea how to use that information at the moment but it should never happened.
		 * So just log the first one region for further investigation.
		 */
		/*
		 LOG_INFO(
		 "FrameId=" << frame->getId() <<
		 "Can't start a tracker for region={ " << ""
		 detectedAndNotStarted.pop_front()->roi() << " }";
		 );
		 */
	}
	if (detectedAndStarted.size() > 0 || lostFaces.size() > 0) {
		// start a new scene
		_scene.since(frame->getTimestamp());
		PFaceList &faces = _scene.getFaces();
		addFacesList(frame, detectedAndStarted, &faces);
		updateFacesList(frame, detectedAndTracked, &faces);
		updateFacesList(frame, lostFaces, &faces);
		cv::Size size = frame->get_mat().size();
		Rectangle rect(size.width, size.height);
		PFrameRegion fr(new FrameRegion(frame, rect));
		LOG_INFO("Set scene frame Id=" << fr->getFrame()->getId());
		_scene.frame(fr);
		normalizeScene(_scene, frame); // HACK
		_listener->onSceneChanged(_scene);
		removeFacesList(lostFaces, &faces);
	} else {
		//if (detectedAndTracked.size() > 0) {
			// update faces frames, if needed
			PFaceList &faces = _scene.getFaces();
			//updateFacesList(frame, detectedAndTracked, &faces);
		//}
		updateFacesList(frame, tracked, &faces);
		cv::Size size = frame->get_mat().size();
		Rectangle rect(size.width, size.height);
		PFrameRegion fr(new FrameRegion(frame, rect));
		normalizeScene(_scene, frame); // HACK
		_scene.frame(fr);
		_listener->onSceneUpdated(_scene);
	}
}

void NaiveSceneDetector::addFacesList(const PFrame &frame,
		const FaceRegionsList &faceRegions, PFaceList *faces) {
	for (FaceRegion fr : faceRegions) {
		PFace pFace = getFace(fr.id(), *faces);
		if (pFace != nullptr) {
			LOG_ERROR(
					"FrameId=" << frame->getId() << " . " << fr.id() << " has been already added");
		} else {
			PFrameRegList regions;
			regions.push_back(FrameRegion(frame, cvRoi_to_rectangle(fr.roi())));
			pFace = PFace(new Face(fr.id(), regions, frame->getTimestamp()));
			faces->push_back(pFace);
		}
	}
}

void NaiveSceneDetector::removeFacesList(const FaceIdsList &lostFaces,
		PFaceList *faces) {
	for (FaceId id : lostFaces) {
		PFaceList::iterator itr = faces->begin();
		bool removed = false;
		while (!removed && itr != faces->end()) {
			if ((*itr)->getId() == id) {
				faces->erase(itr);
				removed = true;
			}
		}
		if (!removed) {
			LOG_ERROR("Can't find " << id);
		}
	}
}

void NaiveSceneDetector::updateFacesList(const PFrame &frame,
		const FaceIdsList &lostFaces, PFaceList *faces) {
	const Timestamp ts = frame->getTimestamp();
	for (FaceId id : lostFaces) {
		PFace pFace = getFace(id, *faces);
		if (pFace.get() == nullptr) {
			LOG_ERROR(
					"FrameId=" << frame->getId() << " . Can't find face " << id);
		} else {
			faces->remove(pFace);
			PFace newFace(new Face(pFace->getId(), pFace->getImages(), pFace->firstTimeCatched(), ts));
			faces->push_back(newFace);
		}
	}
}

void NaiveSceneDetector::updateFacesList(const PFrame &frame,
		const FaceRegionsList &faceRegions, PFaceList *faces) {
	for (FaceRegion fr : faceRegions) {
		PFace pFace = getFace(fr.id(), *faces);
		if (pFace == nullptr) {
			LOG_ERROR(
					"FrameId=" << frame->getId() << " . Can't find face " << fr.id());
		} else {
			// We don't add new regions unless there is no one. or just update existing one
			PFrameRegList fl;
			fl.push_back(FrameRegion(frame, cvRoi_to_rectangle(fr.roi())));
			faces->remove(pFace);
			PFace newFace(new Face(pFace->getId(), fl, pFace->firstTimeCatched(), pFace->lostTime()));
			faces->push_back(newFace);
		}
	}
}

PFace NaiveSceneDetector::getFace(const FaceId &id, const PFaceList &faces) {
	PFace pFace = nullptr;
	// Usualy there are only few faces
	for (PFace candidate : faces) {
		if (candidate->getId() == id) {
			pFace = candidate;
			break;
		}
	}
	return pFace;
}

void NaiveSceneDetector::onStop() {

}

}
