/*
 * model.cpp
 *
 *  Created on: Jan 26, 2017
 *      Author: dmitry
 */

#include "model.hpp"
#include "logger.hpp"
#include <chrono>
#include <boost/thread/locks.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <dlib/matrix/matrix_utilities.h>
#include <algorithm>

using namespace dlib;

namespace fproc {

//==================================== Frame ===================================
Frame::DlibBgrImg& Frame::get_bgr_image() {
	if (_bgr_img.get() == NULL) {
		bgr_image pcvi(new Frame::DlibBgrImg(_mat));
		_bgr_img = std::move(pcvi);
	}
	return *_bgr_img;
}

Frame::DlibRgbImg& Frame::get_rgb_image() {
	if (_rgb_img.get() == NULL) {
		rgb_image pcvi(new Frame::DlibRgbImg(dlib::mat(dlib::cv_image<dlib::rgb_pixel>(_mat))));
		_rgb_img = std::move(pcvi);
	}
	return *_rgb_img;
}

Frame::CvBgrMat& Frame::get_gray_mat() {
	if (grey_mat_.cols != _mat.cols || grey_mat_.rows != _mat.rows) {
		cv::cvtColor(_mat, grey_mat_, CV_BGR2GRAY);
	}
	return grey_mat_;
}

//================================== VideoStream ===============================
void VideoStream::setResolution(int width, int height) {
	_cap->set(CV_CAP_PROP_FRAME_WIDTH, width);
	_cap->set(CV_CAP_PROP_FRAME_HEIGHT, height);
}

//============================== VideoStreamCopier =============================
bool VideoStreamCopier::process() {
	{
        boost::lock_guard<boost::mutex> guard(_lock);
        if (_started) {
        	throw std::runtime_error("the VideoStreamCopier already running");
        }
		_started = true;
	}

	bool result = false;
	LOG_INFO("VideoStreamCopier: Entering processing.");
	try {
		while (_started) {
			PFrame frame = _src->captureFrame();
            if (frame->isEmpty()) {
                LOG_INFO("VideoStreamCopier: got an empty frame. Stop processing");
				stop();
				result = true;
				break;
			}

			if (!_dst->consumeFrame(frame)) {
				LOG_INFO("VideoStreamCopier: consumer returns it is closed...");
				stop();
				result = true;
				break;
			}
		}
	} catch (std::exception &e) {
        LOG_ERROR("VideoStreamCopier: Oops an exception happens: " << e.what());
		stop(); // just in case
	}
	LOG_INFO("VideoStreamCopier: Exit processing.");
	return result;
}

void VideoStreamCopier::stop() {
    boost::lock_guard<boost::mutex> guard(_lock);
	if (!_started) {
		return;
	}
	LOG_INFO("VideoStreamCopier: stop()");
	_started = false;
	_dst->close();
}

//==================================== Misc ===================================

Rectangle toRectangle(const CvRect& cvr) {
	return Rectangle(cvr.x, cvr.y, cvr.x + cvr.width, cvr.y + cvr.height);
}

Rectangle toRectangle(const cv::Rect2d& cvr) {
	return Rectangle((int)cvr.x, (int)cvr.y, (int)(cvr.x + cvr.width), (int)(cvr.y + cvr.height));
}

Rectangle addBorder(const Rectangle& rect, const Size& size, int brdr) {
	return Rectangle(std::max(0l, rect.left() - brdr), std::max(1l, rect.top() - brdr), std::min(long(size.width-1), rect.right() + brdr), std::min(long(size.height-1), rect.bottom() + brdr));
}

CvRect toCvRect(const Rectangle& rect) {
	return cvRect(rect.left(), rect.top(), rect.width(), rect.height());
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

std::string uuid() {
	static boost::uuids::random_generator ug;
	return boost::lexical_cast<std::string>(ug());
};

float distance(const V128D &v1, const V128D &v2) {
	return dlib::length(v1-v2);
};

}

