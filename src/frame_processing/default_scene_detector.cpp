/*
 * default_scene_detector.cpp
 *
 *  Created on: Jan 24, 2017
 *      Author: dmitry
 */

#include <exception>

#include <boost/thread/locks.hpp>

#include "default_scene_detector.hpp"
#include "../logger.hpp"

namespace fproc {

DefaultSceneDetector::DefaultSceneDetector(PVideoStream vstream): 
		      SceneDetector(std::move(vstream),
				    PSceneDetectorListener(new SceneDetectorListener())
				   ){}


DefaultSceneDetector::DefaultSceneDetector(PVideoStream vstream, 
					   PSceneDetectorListener listener): 
					   SceneDetector(std::move(vstream), std::move(listener)) {
}

void DefaultSceneDetector::doProcess(PFrame frame){

}

}
