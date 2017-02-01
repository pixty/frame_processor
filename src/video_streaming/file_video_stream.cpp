/*
 * file_video_stream.cpp
 *
 *  Created on: Jan 30, 2017
 *      Author: dmitry
 */

#include "file_video_stream.hpp"

namespace fproc {

FileVideoStream::FileVideoStream(std::string fileName, bool cycling) :
		VideoStream(
				std::unique_ptr<cv::VideoCapture>(
						new cv::VideoCapture(fileName))) {
	_cycling = cycling;
	_file_name = fileName;
	_started_at = 0;
}

PFrame FileVideoStream::captureFrame() {
	if (_started_at == 0) {
		_started_at = ts_now();
	}

	Frame *frame = new Frame((int)_cap->get(cv::CAP_PROP_POS_FRAMES), _started_at + (Timestamp)_cap->get(cv::CAP_PROP_POS_MSEC));
	*_cap >> frame->get_mat();
	return PFrame(frame);
}

FileVStreamWriter::FileVStreamWriter(std::string& fileName, int fourcc,
		double fps, Size frameSize) :
		_writer(new cv::VideoWriter()) {
	_writer->open(fileName, fourcc, fps, frameSize);
}

void FileVStreamWriter::write(PFrame frame) {
	_writer->write(frame->get_mat());
}

}

