#ifndef SRC_FRAME_PROCESSING1_FILE_SCENE_DETECTOR_LISTENER_HPP_
#define SRC_FRAME_PROCESSING1_FILE_SCENE_DETECTOR_LISTENER_HPP_

#include "../frame_processing1/scene_jsonify.hpp"
#include "../model.hpp"

namespace fproc {
struct FileSceneDetectorListenerCfg {
	FileSceneDetectorListenerCfg() { _filename = "FileSceneDetectorListener.txt";}
	FileSceneDetectorListenerCfg(const std::string &filename): _filename(filename) { }

	const std::string& filename() const { return _filename; }
	void filename(const std::string &filename) { _filename = filename; }
private:
	std::string _filename;
};

class FileSceneDetectorListener: public SceneDetectorListener {
public:
	FileSceneDetectorListener(const FileSceneDetectorListenerCfg &cfg);
	~FileSceneDetectorListener();
	void onSceneChanged(const Scene& scene);
	void onStopped();

private:
	void close();
	std::ofstream _out;
	SceneJsonify _toJson;
};

}
#endif // SRC_FRAME_PROCESSING1_FILE_SCENE_DETECTOR_LISTENER_HPP_
