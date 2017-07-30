/*
 * object_tracker.cpp
 *
 *  Created on: Jun 15, 2017
 *      Author: dmitry
 */

#include "object_detectors.hpp"
#include "../logger.hpp"

namespace fproc {

ObjectTracker::ObjectTracker() {
}

void ObjectTracker::setRegion(PFrameRegion region) {
	_region = region;
}

bool ObjectTracker::startTracking() {
	if (!_region.get()) {
		return false;
	}
	LOG_INFO("ObjectTracker: start tracking");
	_cvTracker = cv::Tracker::create("MEDIANFLOW");
	Frame::CvBgrMat& mat = _region->getFrame()->get_mat();
	CvRect r = toCvRect(_region->getRectangle());
	_region.reset();
	if (!_cvTracker->init(mat, r)) {
		LOG_WARN("ObjectTracker: got non-null region, but cannot init tracker");
		return false;
	}
	return true;
}

PFrameRegion ObjectTracker::track(PFrame pFrame) {
	cv::Rect2d r;
	if (!_cvTracker.get() || !_cvTracker->update(pFrame->get_mat(), r)) {
		return PFrameRegion();
	}
	LOG_WARN("ObjectTracker: Ok");
	return PFrameRegion(new FrameRegion(pFrame, toRectangle(r)));
}

}


