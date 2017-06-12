/*
 * vfile_scene_detector.cpp
 *
 *  Created on: Jan 30, 2017
 *      Author: dmitry
 */

#include "../frame_processing1/vfile_scene_detector.hpp"

#include "../logger.hpp"

namespace fproc {

VFileSceneDetector::VFileSceneDetector( FileVideoStream *fvs, const VFileSceneDetectorCfg &cfg)
    : SceneDetector(PVideoStream(fvs), PSceneDetectorListener(new SceneDetectorListener())),
      _faceDetector(cfg.faceLandmarksModelFilename())
{
	int fourcc = CV_FOURCC('M','J','P','G');
	double fps = fvs->getFps();
	Size size = fvs->getSize();
	LOG_INFO("VFileSceneDetector construct new: inFile=" << fvs->getFileName() << ", outFile=" << cfg.outFile() << ""
			<< " fourcc=" << fourcc << ", fps=" << fps << ", size=" << size);
	_out_stream = std::shared_ptr<FileVStreamWriter>(new FileVStreamWriter(cfg.outFile(), fourcc, fps, size));
}

void VFileSceneDetector::doProcess(PFrame frame) {
	_faceDetector.detectRegions(frame); //TODO: don't write into the frame!
	_out_stream->write(frame);
}

void VFileSceneDetector::onStop() {
	_out_stream->close();
}

}

