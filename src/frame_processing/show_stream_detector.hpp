/*
 * show_sream_detector.hpp
 *
 *  Created on: Jan 26, 2017
 *      Author: dmitry
 */

#ifndef SRC_FRAME_PROCESSING_SHOW_STREAM_DETECTOR_HPP_
#define SRC_FRAME_PROCESSING_SHOW_STREAM_DETECTOR_HPP_

#include "../model.hpp"
#include "../video_streaming/image_window.hpp"
#include "face_detector.hpp"

namespace fproc {
	struct ShowStreamDetectorCfg{
		ShowStreamDetectorCfg(
				   const std::string &faceLandmarksModelFilename=
							std::string("shape_predictor_68_face_landmarks.dat")
				  ):
				  _faceLandmarksModelFilename(faceLandmarksModelFilename)
				  {}
				  
		const std::string & faceLandmarksModelFilename() const{
		  return _faceLandmarksModelFilename;
		}
		void faceLandmarksModelFilename(const std::string &faceLandmarksModelFilename){
		  _faceLandmarksModelFilename = faceLandmarksModelFilename;
		}
	private:
		std::string _faceLandmarksModelFilename;
	};

	class ShowStreamDetector: public SceneDetector {
	public:
		ShowStreamDetector(PVideoStream vstream, 
				   const ShowStreamDetectorCfg &cfg=ShowStreamDetectorCfg());
		virtual ~ShowStreamDetector();

	protected:
		void doProcess(PFrame frame);

		ImageWindow _imgWindow;
		FaceDetector _faceDetector;
	};

} /* namespace fproc */

#endif /* SRC_FRAME_PROCESSING_SHOW_STREAM_DETECTOR_HPP_ */
