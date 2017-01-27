/*
 * file_video_stream.hpp
 *
 *  Created on: Jan 25, 2017
 *      Author: dmitry
 */

#ifndef SRC_VIDEO_STREAMING_FILE_VIDEO_STREAM_HPP_
#define SRC_VIDEO_STREAMING_FILE_VIDEO_STREAM_HPP_

#include "model.hpp"

namespace fproc {

	/*
	 * FileVideoStream - a video stream captured from a file.
	 */
	class FileVideoStream: public VideoStream {
	public:
		/*
		 * Constructs new video stream. The cycling param allows to play the video in a loop non-stop
		 */
		FileVideoStream(std::string fileName, bool cycling):
			VideoStream(std::unique_ptr<cv::VideoCapture>(new cv::VideoCapture(fileName))) {
			_cycling = cycling;
		}
		virtual ~FileVideoStream();

		/*
		 * Captures a frame from the video file. The following conventions is used:
		 * - Frames timestamp is a timestamp when the video starts to play plus duration of
		 * the video to the moment of the mehtod call.
		 *
		 * Throws exception if file not found or cannot be properly decoded.
		 *
		 * Returns NULL if the stream is over and cycling == false
		 */
		virtual PFrame captureFrame();

	private:
		bool _cycling;
	};

}


#endif /* SRC_VIDEO_STREAMING_FILE_VIDEO_STREAM_HPP_ */
