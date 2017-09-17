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
    WebcamVideoStream(const CameraParameters &cameraParameters);

	virtual PFrame captureFrame();
private:
    long _fn;
    const CameraParameters _cameraParameters;

    void setCameraParameters();
    void printActualCameraParameters();
};
}

#endif /* SRC_VIDEO_STREAMING_WEBCAM_VIDEO_STREAM_HPP_ */
