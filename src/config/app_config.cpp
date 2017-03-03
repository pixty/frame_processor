
#include "app_config.hpp"
#include "../video_streaming/webcam_video_stream.hpp"
#include "../video_streaming/file_video_stream.hpp"
#include "../frame_processing/naive_scene_detector_debugger.hpp"
#include "../frame_processing/show_stream_detector.hpp"

namespace fproc {

AppConfig::AppConfig():
	  _videoStreamCfg(CameraStreamCfg()),
	  _sceneDetectorCfg(NaiveSceneDetectorCfg()),
	  _sceneDetectorListenerCfg(FileSceneDetectorListenerCfg())
{}
  
AppConfig::~AppConfig() {}


//-----------------------------------------------------------------------------
// VideoSourceCfg
//-----------------------------------------------------------------------------
pt::ptree VideoStreamCfgToJson::operator()(const CameraStreamCfg & cfg) const{
  pt::ptree val;
  val.put("internalCameraId", cfg.internalCameraId());
  pt::ptree node;  
  node.put_child("CameraStreamCfg", val);
  return node;
}

pt::ptree VideoStreamCfgToJson::operator()(const FileVideoStreamCfg & cfg) const{
  pt::ptree val;
  val.put("filename", cfg.filename());
  val.put("startFrom", cfg.startFrom());
  val.put("stopAfter", cfg.stopAfter());
  val.put("cycling", cfg.cycling());
  pt::ptree node;
  node.put_child("FileVideoStreamCfg", val);
  return node;
}

const Cfgs::VideoStreamCfg VideoStreamCfgJsonify::fromJson(const pt::ptree &root) const{
  boost::optional<const pt::ptree&> value = root.get_child_optional("FileVideoStreamCfg");
  if(value){
    pt::ptree node = value.get();
    FileVideoStreamCfg cfg;
    std::string filename = node.get<std::string>("filename", cfg.filename());
    cfg.filename(filename);
    const int startFrom = node.get<int>("startFrom", cfg.startFrom());
    cfg.startFrom(startFrom);
    const int stopAfter = node.get<int>("stopAfter", cfg.stopAfter());
    cfg.stopAfter(stopAfter);
    const bool cycling = node.get<bool>("cycling", cfg.cycling());
    cfg.cycling(cycling);
    return Cfgs::VideoStreamCfg(cfg);
  }
  value = root.get_child_optional("CameraStreamCfg");
  if(value){
    pt::ptree node = value.get();
    int internalCameraId = node.get<int>("internalCameraId");
    CameraStreamCfg cfg;
    return Cfgs::VideoStreamCfg(cfg);
  }
  CameraStreamCfg cfg;
  return Cfgs::VideoStreamCfg(cfg);
}

PVideoStream VideoStreamFactory::operator()(const CameraStreamCfg & cfg) const{  
  return PVideoStream(new WebcamVideoStream(cfg));
}

PVideoStream VideoStreamFactory::operator()(const FileVideoStreamCfg & cfg) const{
  return PVideoStream(new FileVideoStream(cfg));
}

//-----------------------------------------------------------------------------
// SceneDetectorCfg
//-----------------------------------------------------------------------------
pt::ptree SceneDetectorCfgToJson::operator()(const NaiveSceneDetectorCfg & cfg) const{  
  pt::ptree params_node;
  params_node.put("minFaceSize", cfg.params().minFaceSize());
  params_node.put("maxFaceSize", cfg.params().maxFaceSize());
  params_node.put("iou_thresh", cfg.params().iou_thresh());
  params_node.put("percent_covered_thresh", cfg.params().percent_covered_thresh());
  params_node.put("maxFramesToLooseTrack", cfg.params().maxFramesToLooseTrack());
  params_node.put("maxFaces", cfg.params().maxFaces());
  pt::ptree val;
  val.put_child("NaiveSceneDetectorParameters", params_node);
  val.put("visualDebugging", cfg.visualDebugging());
  pt::ptree node;  
  node.put_child("NaiveSceneDetectorCfg", val);
  return node;  
}

pt::ptree SceneDetectorCfgToJson::operator()(const ShowStreamDetectorCfg & cfg) const{
  pt::ptree node;  
  pt::ptree val;
  val.put("faceLandmarksModelFilename", cfg.faceLandmarksModelFilename());
  node.put_child("ShowStreamDetectorCfg", val);
  return node;
}

pt::ptree SceneDetectorCfgToJson::operator()(const VFileSceneDetectorCfg & cfg) const{
  pt::ptree node;  
  pt::ptree val;
  val.put("faceLandmarksModelFilename", cfg.faceLandmarksModelFilename());
  val.put("outFile", cfg.outFile());
  node.put_child("VFileSceneDetectorCfg", val);
  return node; 
}

const Cfgs::SceneDetectorCfg SceneDetectorCfgJsonify::fromJson(const pt::ptree &root) const{
  boost::optional<const pt::ptree&> value = root.get_child_optional("NaiveSceneDetectorCfg");
  if(value){
    boost::optional<const pt::ptree&> ps_node = value.get().get_child_optional("NaiveSceneDetectorParameters");
    NaiveSceneDetectorCfg cfg;
    if(ps_node){
      const pt::ptree& node = ps_node.get();
      cfg.params().minFaceSize(node.get<int>("minFaceSize", 
					   cfg.params().minFaceSize()));
      cfg.params().maxFaceSize(node.get<int>("maxFaceSize", 
					   cfg.params().maxFaceSize()));
      cfg.params().iou_thresh(node.get<double>("iou_thresh", 
					     cfg.params().iou_thresh()));
      cfg.params().percent_covered_thresh(node.get<double>("percent_covered_thresh", 
							 cfg.params().percent_covered_thresh()));
      cfg.params().maxFramesToLooseTrack(node.get<int>("maxFramesToLooseTrack", 
						     cfg.params().maxFramesToLooseTrack()));
      cfg.params().maxFaces(node.get<int>("maxFaces", 
					cfg.params().maxFaces()));
    }
    cfg.visualDebugging(value.get().get<bool>("visualDebugging", cfg.visualDebugging()));
    return Cfgs::SceneDetectorCfg(cfg);
  }
  value = root.get_child_optional("ShowStreamDetectorCfg");
  if(value){
    ShowStreamDetectorCfg cfg;
    cfg.faceLandmarksModelFilename(value.get().get<std::string>("faceLandmarksModelFilename", 
								cfg.faceLandmarksModelFilename()));
    return Cfgs::SceneDetectorCfg(cfg);
  }
  value = root.get_child_optional("VFileSceneDetectorCfg");
  if(value){
    VFileSceneDetectorCfg cfg;
    cfg.faceLandmarksModelFilename(value.get().get<std::string>("faceLandmarksModelFilename", 
								cfg.faceLandmarksModelFilename()));
    cfg.outFile(value.get().get<std::string>("outFile", 
					     cfg.outFile()));

    return Cfgs::SceneDetectorCfg(cfg);
  }
  NaiveSceneDetectorCfg cfg;
  return Cfgs::SceneDetectorCfg(cfg);  
}

PSceneDetector SceneDetectorFactory::operator()(const NaiveSceneDetectorCfg & cfg){
  NaiveSceneDetector::PDebugger dbg (cfg.visualDebugging() ?
					new NaiveSceneDetectorDebugger():
					new NaiveSceneDetector::Debugger()
				    );
  return PSceneDetector(new NaiveSceneDetector(std::move(_vs), std::move(_sdl), std::move(dbg), cfg.params()));
}

PSceneDetector SceneDetectorFactory::operator()(const ShowStreamDetectorCfg & cfg){
  return PSceneDetector(new ShowStreamDetector(std::move(_vs), cfg));
}

PSceneDetector SceneDetectorFactory::operator()(const VFileSceneDetectorCfg & cfg){
  FileVideoStream *fvs = dynamic_cast<FileVideoStream *>(_vs.release());
  if(fvs == nullptr){
    throw std::invalid_argument( "VFileSceneDetector requires a FileVideoStream." );
  }
  return PSceneDetector(new VFileSceneDetector(fvs, cfg));
}
//-----------------------------------------------------------------------------
// SceneDetectorListenerCfg
//-----------------------------------------------------------------------------
pt::ptree SceneDetectorListenerCfgToJson::operator()(const SceneNilListenerCfg & cfg) const{
  pt::ptree node;  
  pt::ptree val;
  node.put_child("SceneNilListenerCfg", val);
  return node;
}

pt::ptree SceneDetectorListenerCfgToJson::operator()(const FileSceneDetectorListenerCfg & cfg) const{
  pt::ptree val;
  val.put("filename", cfg.filename());
  pt::ptree node;  
  node.put_child("FileSceneDetectorListenerCfg", val);
  return node;  
}

const Cfgs::SceneDetectorListenerCfg SceneDetectorListenerCfgJsonify::fromJson(const pt::ptree &root) const{
    boost::optional<const pt::ptree&> value = root.get_child_optional("FileSceneDetectorListenerCfg");
  if(value){
    FileSceneDetectorListenerCfg cfg;
    cfg.filename(value.get().get<std::string>("filename", cfg.filename()));
    return Cfgs::SceneDetectorListenerCfg(cfg);
  }
  value = root.get_child_optional("SceneNilListenerCfg");
  if(value){
    SceneNilListenerCfg cfg;
    return Cfgs::SceneDetectorListenerCfg(cfg);
  }
  FileSceneDetectorListenerCfg cfg;
  return Cfgs::SceneDetectorListenerCfg(cfg);  
}

PSceneDetectorListener SceneDetectorListenerFactory::operator()(const SceneNilListenerCfg & cfg) const{
  return PSceneDetectorListener(new SceneDetectorListener());
}

PSceneDetectorListener SceneDetectorListenerFactory::operator()(const FileSceneDetectorListenerCfg & cfg) const{
  return PSceneDetectorListener(new FileSceneDetectorListener(cfg));
}

//-----------------------------------------------------------------------------
// AppConfig
//-----------------------------------------------------------------------------
void AppConfig::toJson(std::ostream &out, bool pretty) const{
  pt::ptree root;
  toJson(&root);
  pt::write_json(out, root, pretty);
}

void AppConfig::toJson(const std::string &filename, bool pretty) const{
  pt::ptree root;
  toJson(&root);
  pt::write_json(filename, root, std::locale(), pretty);
}

void AppConfig::toJson(pt::ptree *root) const{
  pt::ptree vscNode = _videoStreamCfg.apply_visitor(_videoStreamCfgJsonify.toJson());
  root->put_child("VideoSourceCfg", vscNode); 
  pt::ptree sdNode = _sceneDetectorCfg.apply_visitor(_sceneDetectorCfgJsonify.toJson());
  root->put_child("SceneDetectorCfg", sdNode); 
  pt::ptree sdlNode = _sceneDetectorListenerCfg.apply_visitor(_sceneDetectorListenerCfgJsonify.toJson());
  root->put_child("SceneDetectorListenerCfg", sdlNode); 
}

void AppConfig::fromJson(const std::string &filename){
    if(!filename.empty()){
      std::ifstream in(filename.c_str());
      fromJson(in);
    }
}

void AppConfig::fromJson(std::istream &in){
    pt::ptree root;
    pt::read_json(in, root);
    
    boost::optional<pt::ptree&> value = root.get_child_optional("VideoSourceCfg");
    if(value){
      _videoStreamCfg = _videoStreamCfgJsonify.fromJson(value.get());
    }else{
      _videoStreamCfg = CameraStreamCfg();
    }
    value = root.get_child_optional("SceneDetectorCfg");
    if(value){
      _sceneDetectorCfg = _sceneDetectorCfgJsonify.fromJson(value.get());
    }else{
      _sceneDetectorCfg = NaiveSceneDetectorCfg();
    }
    value = root.get_child_optional("SceneDetectorListenerCfg");
    if(value){
      _sceneDetectorListenerCfg = _sceneDetectorListenerCfgJsonify.fromJson(value.get());
    }else{
      _sceneDetectorListenerCfg = FileSceneDetectorListenerCfg();
    }
}

PSceneDetector AppConfig::createSceneDetector() const{
  PVideoStream vs = _videoStreamCfg.apply_visitor(_videoStreamFactory);
  PSceneDetectorListener sdl = _sceneDetectorListenerCfg.apply_visitor(_sceneDetectorListenerFactory);
  SceneDetectorFactory factory = SceneDetectorFactory(std::move(vs), std::move(sdl));
  return _sceneDetectorCfg.apply_visitor(factory);
}

void DefaultCfgs::create(const std::string &filenamePrefix){
  AppConfig cfg;
  std::cout << "Generate config exsamples.\n See ";
  std::string camera = filenamePrefix + "_camera.json";
  std::cout << camera;
  cfg.toJson(camera);
  cfg.videoStreamCfg(FileVideoStreamCfg());
  
  std::string file = filenamePrefix + "_file.json";
  std::cout << ", " << file;
  cfg.toJson(file);
  
  cfg.sceneDetectorListenerCfg(SceneNilListenerCfg());
  cfg.sceneDetectorCfg(ShowStreamDetectorCfg());
  std::string file_nil_listener = filenamePrefix + "_file_nil_listener.json";
  std::cout << ", " << file_nil_listener;
  cfg.toJson(file_nil_listener);
  
  cfg.sceneDetectorCfg(VFileSceneDetectorCfg());
  std::string vfile_scene_detector = filenamePrefix + "_vfile_scene_detector.json";
  std::cout << ", " << vfile_scene_detector << " files for details.\n";
  cfg.toJson(vfile_scene_detector);
  std::cout << "Completed.\n";
}

}