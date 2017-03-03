#include "file_scene_detector_listener.hpp"

namespace fproc {

FileSceneDetectorListener::FileSceneDetectorListener(const FileSceneDetectorListenerCfg &cfg)
{
  _out.open(cfg.filename());
}

FileSceneDetectorListener::~FileSceneDetectorListener()
{
  close();
}

void FileSceneDetectorListener::onSceneChanged(const Scene& scene){
  _out << _toJson(scene, false) << "\n";
}

void FileSceneDetectorListener::onStopped(){
  close();
}

void FileSceneDetectorListener::close(){
  if(_out.is_open()){
    _out.flush();
    _out.close();
  }  
}

}