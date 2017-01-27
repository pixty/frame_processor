/*
 * webcam_video_stream.hpp
 *
 *  Created on: Jan 25, 2017
 *      Author: dmitry
 */

#ifndef SRC_VIDEO_STREAMING_WEBCAM_VIDEO_STREAM_HPP_
#define SRC_VIDEO_STREAMING_WEBCAM_VIDEO_STREAM_HPP_

#include "../model.hpp"

namespace fproc {

	/*
	 * WebcamVideoStream - a video stream captured from a build-in camera
	 */
	class WebcamVideoStream: public VideoStream {
	public:
		WebcamVideoStream(): VideoStream(std::unique_ptr<cv::VideoCapture>(new cv::VideoCapture())) {
			_cap->open(0);
		}

		virtual PFrame captureFrame();

		/* Other methods and members are not defined yet */
	};
}

#endif /* SRC_VIDEO_STREAMING_WEBCAM_VIDEO_STREAM_HPP_ */
