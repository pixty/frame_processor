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

using namespace dlib;

namespace fproc {

//==================================== Frame ===================================
Frame::DlibBgrImg& Frame::get_cv_image() {
	if (_cv_img.get() == NULL) {
		pcv_image pcvi(new Frame::DlibBgrImg(_mat));
		_cv_img = std::move(pcvi);
	}
	return *_cv_img;
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

