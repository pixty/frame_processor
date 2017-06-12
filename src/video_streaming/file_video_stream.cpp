/*
 * file_video_stream.cpp
 *
 *  Created on: Jan 30, 2017
 *      Author: dmitry
 */

#include "file_video_stream.hpp"
#include "../logger.hpp"

namespace fproc {

FileVideoStream::FileVideoStream(const std::string& fileName, int firstFrame,
		int frames) :
		VideoStream(
				std::unique_ptr<cv::VideoCapture>(
						new cv::VideoCapture(fileName))), _started_at(
				0), _skipped(false) {
	_startFrom = firstFrame <= 0 ? 0 : firstFrame;
	_stopAfter = frames > 0 ? _startFrom + frames : -1;
}

PFrame FileVideoStream::captureFrame() {
	if (_started_at == 0) {
		_started_at = ts_now();
	}
	skip();
	int pos = (int) _cap->get(cv::CAP_PROP_POS_FRAMES);
	Frame *frame = new Frame(pos, _started_at + (Timestamp) _cap->get(cv::CAP_PROP_POS_MSEC));
	if (_stopAfter < 0 || pos < _stopAfter) {
		*_cap >> frame->get_mat();
	}
	return PFrame(frame);
}

void FileVideoStream::skip() {
	if (!_skipped) {
		int pos = (int) _cap->get(cv::CAP_PROP_POS_FRAMES);
		int toSkip = _startFrom - pos;
		if (toSkip > 0) {
			cv::Mat tmp;
			for (; toSkip > 0; toSkip--) {
				*_cap >> tmp;
			}
		}
		_skipped = true;
	}
}

FileVStreamWriter::FileVStreamWriter(const std::string& fileName, int fourcc,
		double fps, Size frameSize) :
		_writer(new cv::VideoWriter()) {
	_writer->open(fileName, fourcc, fps, frameSize);
}

bool FileVStreamWriter::consumeFrame(PFrame frame) {
	_writer->write(frame->get_mat());
	return _writer->isOpened();
}

}

