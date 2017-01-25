/*
 * stream.hpp
 *
 *  Created on: Jan 16, 2017
 *      Author: dmitry
 */

#ifndef SRC_STREAM_HPP_
#define SRC_STREAM_HPP_

#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>

namespace fp {

	class WebcamVideoStream;
	class VideoFrame {
	public:
		VideoFrame() {}
		~VideoFrame() {}
        cv::Mat& frame(){
            return _cv_mat;
        }
	private:
        long id;
        long msec;
        std::unique_ptr <cv::Mat> _cv_mat;
		friend WebcamVideoStream;
	};

	struct VideoStream {
		virtual VideoFrame captureFrame() = 0;
		virtual ~VideoStream() {};
	};

	class WebcamVideoStream: public VideoStream {
	public:
        WebcamVideoStream():_cap(new cv::VideoCapture()) {
            _cap->open(0);
        }
                WebcamVideoStream(string filename) { _cap = new cv::VideoCapture(filename);}
		virtual ~WebcamVideoStream() {}

		virtual VideoFrame* captureFrame() {
			VideoFrame* fm = new VideoFrame();
			_cap >> fm->_cv_mat;
            fm->msec = _cap.get(cv::CAP_PROP_POS_MSEC);
            fm->id = _cap.get(cv::CAP_PROP_POS_FRAMES);
			return fm;
		}
	private:
        std::unique_ptr<cv::VideoCapture> _cap;
	};
}



#endif /* SRC_STREAM_HPP_ */
