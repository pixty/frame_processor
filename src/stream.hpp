/*
 * stream.hpp
 *
 *  Created on: Jan 16, 2017
 *      Author: dmitry
 */

#ifndef SRC_STREAM_HPP_
#define SRC_STREAM_HPP_

#include <opencv2/opencv.hpp>

namespace fp {

	class WebcamVideoStream;
	class VideoFrame {
	public:
		VideoFrame() {}
		~VideoFrame() {}
	private:

		cv::Mat _cv_mat;
		friend WebcamVideoStream;
	};

	struct VideoStream {
		virtual VideoFrame captureFrame() = 0;
		virtual ~VideoStream() {};
	};

	class WebcamVideoStream: public VideoStream {
	public:
		WebcamVideoStream() { _cap = new cv::VideoCapture(0);}
		virtual ~WebcamVideoStream() {}

		virtual VideoFrame* captureFrame() {
			VideoFrame* fm = new VideoFrame();
			_cap >> fm->_cv_mat;
			return fm;
		}
	private:
		std::shared_ptr<cv::VideoCapture> _cap;
	};
}



#endif /* SRC_STREAM_HPP_ */
