/*
 * vfile_scene_detector.hpp
 *
 *  Created on: Jan 30, 2017
 *      Author: dmitry
 */

#ifndef SRC_FRAME_PROCESSING_VFILE_SCENE_DETECTOR_HPP_
#define SRC_FRAME_PROCESSING_VFILE_SCENE_DETECTOR_HPP_

#include "../model.hpp"
#include "../video_streaming/file_video_stream.hpp"
#include "face_detector.hpp"

namespace fproc {

	/*
	 * VFileSceneDetector is a scene detector which accepts input file, recognizes faces on each
	 * frame and writes results into output file
	 */
	class VFileSceneDetector: public SceneDetector {
	public:
		VFileSceneDetector(FileVideoStream& fvs, std::string outFile);
		virtual ~VFileSceneDetector() {}

	protected:
		void doProcess(PFrame frame);
		void onStop();

		FaceDetector faceDetector;

	private:
		std::shared_ptr<FileVStreamWriter> _out_stream;
	};

}

#endif /* SRC_FRAME_PROCESSING_VFILE_SCENE_DETECTOR_HPP_ */
