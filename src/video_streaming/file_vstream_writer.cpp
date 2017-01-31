/*
 * file_video_stream.cpp
 *
 *  Created on: Jan 30, 2017
 *      Author: dmitry
 */

#include "file_video_stream.hpp"

namespace fproc {
FileVStreamWriter::FileVStreamWriter(std::string& fileName, int fourcc,
		double fps, Size frameSize): _writer(new cv::VideoWriter(fileName, fourcc, fps, frameSize)) {
}

void FileVStreamWriter::write(PFrame frame) {
	*_writer << frame->get_mat();
}

}

