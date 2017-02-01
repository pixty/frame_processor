/*
 * webcam_video_stream.cpp
 *
 *  Created on: Jan 26, 2017
 *      Author: dmitry
 */

#include "webcam_video_stream.hpp"

namespace fproc {

	PFrame WebcamVideoStream::captureFrame() {
		Frame* frame = new Frame(1, ts_now());
		*_cap >> frame->get_mat();
		return PFrame(frame);
	}

}