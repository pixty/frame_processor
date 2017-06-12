/*
 * webcam_video_stream.hpp
 *
 *  Created on: Jan 25, 2017
 *      Author: dmitry
 */

#ifndef SRC_VIDEO_STREAMING_WEBCAM_VIDEO_STREAM_HPP_
#define SRC_VIDEO_STREAMING_WEBCAM_VIDEO_STREAM_HPP_

#include "../model.hpp"
#include "../logger.hpp"

namespace fproc {

/*
 * WebcamVideoStream - a video stream captured from a build-in camera
 */
class WebcamVideoStream: public VideoStream {
public:
	WebcamVideoStream():
			VideoStream(
					std::unique_ptr<cv::VideoCapture>(new cv::VideoCapture())), _fn(
					ts_now()) {
		_cap->open(0);
		LOG_INFO("Creating WebcamVideoStream, first frame id=" << _fn);
	}

	virtual PFrame captureFrame();
private:
	long _fn;
};
}

#endif /* SRC_VIDEO_STREAMING_WEBCAM_VIDEO_STREAM_HPP_ */
