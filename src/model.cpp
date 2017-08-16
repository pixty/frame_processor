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

std::vector<uchar>& Frame::png_buf() {
	if (formatted_buf_.size() > 0) {
		return formatted_buf_;
	}
	formatted_buf_.reserve(2000000);
	std::vector<int> compression_params;
	compression_params.push_back(cv::IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(9);

	try {
		cv::imencode(".png", _mat, formatted_buf_, compression_params);
	} catch (cv::Exception& ex) {
		LOG_ERROR("Exception converting image to PNG format: %s\n" << ex.what());
		formatted_buf_.clear();
	}
	return formatted_buf_;
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

