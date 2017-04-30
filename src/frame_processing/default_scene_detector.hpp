/*
 * default_scene_detector.hpp
 *
 *  Created on: Jan 25, 2017
 *      Author: dmitry
 */

#ifndef SRC_FRAME_PROCESSING_DEFAULT_SCENE_DETECTOR_HPP_
#define SRC_FRAME_PROCESSING_DEFAULT_SCENE_DETECTOR_HPP_

#include "../model.hpp"

namespace fproc {

	/*
	 * DefaultSceneDetector is a default implementation of the SceneDetector. It accepts the detector
	 * listener, what allows to have a scene-change notification mechanism.
	 */
	class DefaultSceneDetector: public SceneDetector {
	public:
		DefaultSceneDetector(PVideoStream vstream);
		DefaultSceneDetector(PVideoStream vstream, PSceneDetectorListener listener);

	protected:
		void doProcess(PFrame frame);
	};

}

#endif /* SRC_FRAME_PROCESSING_DEFAULT_SCENE_DETECTOR_HPP_ */
