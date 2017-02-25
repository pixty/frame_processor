/*
 * model.cpp
 *
 *  Created on: Jan 26, 2017
 *      Author: dmitry
 */

#include "model.hpp"
#include <chrono>

using namespace dlib;

namespace fproc {

Frame::DlibBgrImg& Frame::get_cv_image() {
	if (_cv_img.get() == NULL) {
		pcv_image pcvi(new Frame::DlibBgrImg(_mat));
		_cv_img = std::move(pcvi);
	}
	return *_cv_img;
}

Timestamp ts_now() {
	using namespace std::chrono;

	system_clock::time_point tp = system_clock::now();
	system_clock::duration dtn = tp.time_since_epoch();

	return duration_cast<milliseconds>(dtn).count();
}

Size VideoStream::getSize() {
	int width = (int)_cap->get(cv::CAP_PROP_FRAME_WIDTH);
	int height = (int)_cap->get(cv::CAP_PROP_FRAME_HEIGHT);
	return Size(width, height);
}

}

