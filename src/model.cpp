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

cv_image<bgr_pixel>& Frame::get_cv_image() {
	if (_cv_img.get() == NULL) {
		pcv_image pcvi(new cv_image<bgr_pixel>(_mat));
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

}

