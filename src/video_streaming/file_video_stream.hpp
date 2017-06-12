/*
 * file_video_stream.hpp
 *
 *  Created on: Jan 25, 2017
 *      Author: dmitry
 */

#ifndef SRC_VIDEO_STREAMING_FILE_VIDEO_STREAM_HPP_
#define SRC_VIDEO_STREAMING_FILE_VIDEO_STREAM_HPP_

#include "../model.hpp"

namespace fproc {
/*
 * FileVideoStream - a video stream captured from a file.
 */
class FileVideoStream: public VideoStream {
public:
	/*
	 * Constructs new video stream. It starts processing from firstFrame and processing only frames (if positive), or till the end of the stream.
	 */
	FileVideoStream(const std::string& fileName, int firstFrame, int frames);
	virtual ~FileVideoStream() {
	}

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
	inline std::string& getFileName() {
		return _filename;
	}

private:
	void skip();
	Timestamp _started_at;
	std::string _filename;
	int _startFrom;
	int _stopAfter;
	bool _skipped;
};

class FileVStreamWriter: VideoStreamConsumer {
public:
	FileVStreamWriter(const std::string& fileName, int fourcc, double fps,
			Size frameSize);
	virtual ~FileVStreamWriter() {
	}

	bool consumeFrame(PFrame);
	void close() {
		_writer->release();
	}
private:
	std::unique_ptr<cv::VideoWriter> _writer;
};
}

#endif /* SRC_VIDEO_STREAMING_FILE_VIDEO_STREAM_HPP_ */
