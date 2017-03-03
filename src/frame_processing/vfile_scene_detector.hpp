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

	struct VFileSceneDetectorCfg {
		VFileSceneDetectorCfg(const std::string &outFile=std::string("noname.mkv"),
				   const std::string &faceLandmarksModelFilename=
							std::string("shape_predictor_68_face_landmarks.dat")
				  ):
				  _outFile(outFile), 
				  _faceLandmarksModelFilename(faceLandmarksModelFilename)
				  {}
				  
		const std::string & faceLandmarksModelFilename() const{
		  return _faceLandmarksModelFilename;
		}
		void faceLandmarksModelFilename(const std::string &faceLandmarksModelFilename){
		  _faceLandmarksModelFilename = faceLandmarksModelFilename;
		}
		
		const std::string & outFile() const{
		  return _outFile;
		}
		void outFile(const std::string &outFile){
		  _outFile = outFile;
		}
	private:
		std::string _faceLandmarksModelFilename;
		std::string _outFile;
	};
  
	/*
	 * VFileSceneDetector is a scene detector which accepts input file, recognizes faces on each
	 * frame and writes results into output file
	 */
	class VFileSceneDetector: public SceneDetector {
	public:
		VFileSceneDetector(FileVideoStream *fvs, const VFileSceneDetectorCfg &cfg);
		virtual ~VFileSceneDetector() {}

	protected:
		void doProcess(PFrame frame);
		void onStop();

		FaceDetector _faceDetector;

	private:
		std::shared_ptr<FileVStreamWriter> _out_stream;
	};

}

#endif /* SRC_FRAME_PROCESSING_VFILE_SCENE_DETECTOR_HPP_ */
