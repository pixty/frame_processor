#include "../frame_processing/object_detectors.hpp"
#include <opencv2/core/cvstd.hpp>
#include "../frame_processing/haarcascade_frontalface_default_xml.hpp"

namespace fproc {

HaarFaceDetector::HaarFaceDetector(const int minFaceSize, const int maxFaceSize) :
		_minFaceSize(minFaceSize, minFaceSize), _maxFaceSize(maxFaceSize,
				maxFaceSize) {
	cv::FileStorage fs(haarcascade_frontalface_default_xml,
			cv::FileStorage::READ | cv::FileStorage::MEMORY);
	_cvFaceDetector.read(fs.getFirstTopLevelNode());
}

const FRList& HaarFaceDetector::detectRegions(PFrame pFrame) {
	std::vector<CvRect> objects;
	Frame::CvBgrMat& frame = pFrame->get_mat();

	if (_maxFaceSize.height <= 0 || _minFaceSize.height <= 0) {
		_cvFaceDetector.detectMultiScale(frame, objects, 1.3);
	} else {
		_cvFaceDetector.detectMultiScale(frame, objects, 1.3, 3, 0,
				_minFaceSize, _maxFaceSize);
	}

	_objects.clear();
	for(auto const& cr: objects) {
		Rectangle r = toRectangle(cr);
		PFrameRegion pfr(new FrameRegion(pFrame, r));
		_objects.push_back(pfr);
	}

	return _objects;
}

}
