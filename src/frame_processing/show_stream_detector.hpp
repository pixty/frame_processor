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

namespace fproc {

	class ShowStreamDetector: public SceneDetector {
	public:
		ShowStreamDetector(VideoStream& vstream);
		virtual ~ShowStreamDetector();

	protected:
		void doProcess(PFrame &frame);

		ImageWindow imgWindow;
	};

} /* namespace fproc */

#endif /* SRC_FRAME_PROCESSING_SHOW_STREAM_DETECTOR_HPP_ */
