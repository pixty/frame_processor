#include "file_scene_detector_listener.hpp"

#include "../logger.hpp"
#include "../fpcp/fpcp.hpp"

namespace fproc {

FileSceneDetectorListener::FileSceneDetectorListener(
		const FileSceneDetectorListenerCfg &cfg) {
	_out.open(cfg.filename());
}

FileSceneDetectorListener::~FileSceneDetectorListener() {
	close();
}

void FileSceneDetectorListener::onSceneChanged(const Scene& scene) {
	LOG_INFO("scene json = " << fpcp::to_json(scene));
	_out << _toJson(scene, false) << "\n";
	_out.flush();
}

void FileSceneDetectorListener::onStopped() {
	close();
}

void FileSceneDetectorListener::close() {
	if (_out.is_open()) {
		_out.close();
	}
}

}
