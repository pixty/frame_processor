#include "../frame_processing/object_detectors.hpp"

namespace fproc {

HogFaceDetector::HogFaceDetector() :
		_detector(dlib::get_frontal_face_detector()) {
}

PFrameRegList& HogFaceDetector::detectRegions(PFrame pFrame) {
	std::vector<Rectangle> objs = _detector(pFrame->get_bgr_image());

	_objects.clear();
	for (Rectangle& r : objs) {
		PFrameRegion pfr(new FrameRegion(pFrame, r));
		_objects.push_back(pfr);
	}

	return _objects;
}

PFrameRegList& HogFaceDetector::detectRegions(PFrame pFrame,
		const std::vector<Rectangle>& suggested_rects) {
	_objects.clear();
	for (const Rectangle& r : suggested_rects) {
		std::vector<Rectangle> faces = _detector(
				dlib::sub_image(pFrame->get_bgr_image(), r));
		if (faces.size() > 0) {
			// naive approach: just pick the first one up
			Rectangle nr = dlib::translate_rect(faces[0], r.left(), r.top());
			PFrameRegion pfr(new FrameRegion(pFrame, nr));
			_objects.push_back(pfr);
		}
	}
	return _objects;
}

}

