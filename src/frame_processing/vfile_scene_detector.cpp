/*
 * vfile_scene_detector.cpp
 *
 *  Created on: Jan 30, 2017
 *      Author: dmitry
 */

#include "vfile_scene_detector.hpp"
#include "../logger.hpp"

namespace fproc {

VFileSceneDetector::VFileSceneDetector(FileVideoStream& fvs, std::string out): SceneDetector(fvs , nil_sc_detecor_listener) {
	int fourcc = CV_FOURCC('M','J','P','G');
	double fps = fvs.getFps();
	Size size = fvs.getSize();
	LOG_INFO("VFileSceneDetector construct new: inFile=" << fvs.getFileName() << ", outFile=" << out << ""
			<< " fourcc=" << fourcc << ", fps=" << fps << ", size=" << size);
	_out_stream = std::shared_ptr<FileVStreamWriter>(new FileVStreamWriter(out, fourcc, fps, size));
}

void VFileSceneDetector::doProcess(PFrame frame) {
	faceDetector.detectRegions(frame); //TODO: don't write into the frame!
	_out_stream->write(frame);
}

void VFileSceneDetector::onStop() {
	_out_stream->close();
}

}

