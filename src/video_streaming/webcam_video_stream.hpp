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

	struct CameraStreamCfg {
	  constexpr static int DEFAULT_CAMERA_ID = 0;
	  
	  CameraStreamCfg(const int internalCameraId = DEFAULT_CAMERA_ID)
		:_internalCameraId(internalCameraId)
		{}
	      
	    const int internalCameraId() const {return _internalCameraId;}
	    void internalCameraId(const int internalCameraId) {_internalCameraId = internalCameraId;}
	  private:
	    int _internalCameraId;
	};

	/*
	 * WebcamVideoStream - a video stream captured from a build-in camera
	 */
	class WebcamVideoStream: public VideoStream {
	public:
		WebcamVideoStream(const CameraStreamCfg &cfg = CameraStreamCfg()): VideoStream(std::unique_ptr<cv::VideoCapture>(new cv::VideoCapture())) {
			_cap->open(cfg.internalCameraId());
		}

		virtual PFrame captureFrame();

		/* Other methods and members are not defined yet */
	};
}

#endif /* SRC_VIDEO_STREAMING_WEBCAM_VIDEO_STREAM_HPP_ */
