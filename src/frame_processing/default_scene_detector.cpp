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

DefaultSceneDetector::DefaultSceneDetector(VideoStream& vstream): SceneDetector(vstream, nil_sc_detecor_listener) {
}


DefaultSceneDetector::DefaultSceneDetector(VideoStream& vstream, SceneDetectorListener& listener): SceneDetector(vstream, listener) {

}

void DefaultSceneDetector::doProcess(PFrame frame){

}

}

