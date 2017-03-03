#ifndef SRC_CONFIG_APP_CONFIG_HPP_
#define SRC_CONFIG_APP_CONFIG_HPP_

#include <boost/variant.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "../model.hpp"

#include "../video_streaming/webcam_video_stream.hpp"
#include "../video_streaming/file_video_stream.hpp"

#include "../frame_processing/file_scene_detector_listener.hpp"
#include "../frame_processing/naive_scene_detector.hpp"
#include "../frame_processing/show_stream_detector.hpp"
#include "../frame_processing/vfile_scene_detector.hpp"

namespace fproc {
  struct NaiveSceneDetectorCfg{
    NaiveSceneDetectorCfg():_enableVisualDebugging(true){}
    
    const NaiveSceneDetectorParameters& params() const{return _params;}
    NaiveSceneDetectorParameters& params(){return _params;}
    
    void params(const NaiveSceneDetectorParameters &params){_params = params;}
    
    bool visualDebugging()const{return _enableVisualDebugging;}
    
    void visualDebugging(bool enableVisualDebugging){
      _enableVisualDebugging = enableVisualDebugging;
    }
  private:
    NaiveSceneDetectorParameters _params;
    bool _enableVisualDebugging;
  };
  
  struct SceneNilListenerCfg{};
  
  struct Cfgs {
    typedef boost::variant< CameraStreamCfg, FileVideoStreamCfg > VideoStreamCfg;
    typedef boost::variant< NaiveSceneDetectorCfg, ShowStreamDetectorCfg, VFileSceneDetectorCfg > SceneDetectorCfg;
    typedef boost::variant< SceneNilListenerCfg, FileSceneDetectorListenerCfg > SceneDetectorListenerCfg;  
  };

  namespace pt = boost::property_tree;

  struct VideoStreamCfgToJson : public boost::static_visitor<pt::ptree>{
      pt::ptree operator()(const CameraStreamCfg & cfg) const;
      pt::ptree operator()(const FileVideoStreamCfg & cfg) const;
  };

  struct VideoStreamFactory : public boost::static_visitor<PVideoStream>{
      PVideoStream operator()(const CameraStreamCfg & cfg) const;
      PVideoStream operator()(const FileVideoStreamCfg & cfg) const;
  };
  
  struct VideoStreamCfgJsonify {
    const VideoStreamCfgToJson& toJson() const {return _toJson;}
    const Cfgs::VideoStreamCfg fromJson(const pt::ptree &root) const;
  private:
    VideoStreamCfgToJson _toJson;
  };
  
  struct SceneDetectorCfgToJson : public boost::static_visitor<pt::ptree>{
    pt::ptree operator()(const NaiveSceneDetectorCfg & cfg) const;
    pt::ptree operator()(const ShowStreamDetectorCfg & cfg) const;
    pt::ptree operator()(const VFileSceneDetectorCfg & cfg) const;
  };

  struct SceneDetectorCfgJsonify{
    const SceneDetectorCfgToJson& toJson()const{ return _toJson;}
    const Cfgs::SceneDetectorCfg fromJson(const pt::ptree &root) const;
  private:
    SceneDetectorCfgToJson _toJson;
  };

  struct SceneDetectorFactory : public boost::static_visitor<PSceneDetector>{
    SceneDetectorFactory(PVideoStream vs, 
			 PSceneDetectorListener sdl):
			_vs(std::move(vs)),
			_sdl(std::move(sdl))
			{}
    PSceneDetector operator()(const NaiveSceneDetectorCfg & cfg);
    PSceneDetector operator()(const ShowStreamDetectorCfg & cfg);
    PSceneDetector operator()(const VFileSceneDetectorCfg & cfg);
  private:
    PVideoStream _vs;
    PSceneDetectorListener _sdl;
  };
  
  struct SceneDetectorListenerCfgToJson : public boost::static_visitor<pt::ptree>{
    pt::ptree operator()(const SceneNilListenerCfg & cfg) const;
    pt::ptree operator()(const FileSceneDetectorListenerCfg & cfg) const;
  };

  struct SceneDetectorListenerCfgJsonify{
    const struct SceneDetectorListenerCfgToJson& toJson()const {return _toJson;}
    const Cfgs::SceneDetectorListenerCfg fromJson(const pt::ptree &root) const;
  private:
    SceneDetectorListenerCfgToJson _toJson;
  };

  struct SceneDetectorListenerFactory : public boost::static_visitor<PSceneDetectorListener>{
    PSceneDetectorListener operator()(const SceneNilListenerCfg & cfg) const;
    PSceneDetectorListener operator()(const FileSceneDetectorListenerCfg & cfg) const;
  };
  
  class AppConfig
  {
  public:  
    AppConfig();
    ~AppConfig();
    
    void fromJson(std::istream &in);
    void fromJson(const std::string &filename);
      
    friend std::ostream &operator<<( std::ostream &out, const AppConfig &cfg ) { 
	 cfg.toJson(out);
         return out;
      }

    friend std::istream &operator>>( std::istream  &in, AppConfig &cfg ) { 
         cfg.fromJson(in);
         return in;
    }
    void toJson(std::ostream &out, bool pretty=true) const;
    void toJson(const std::string &filename, bool pretty=true) const;
    
    PSceneDetector createSceneDetector() const;
    
    void videoStreamCfg(const Cfgs::VideoStreamCfg &videoSource)
	  {_videoStreamCfg = videoSource;}
    void sceneDetectorCfg(const Cfgs::SceneDetectorCfg &sceneDetector)
	  {_sceneDetectorCfg = sceneDetector;}
    void sceneDetectorListenerCfg(const Cfgs::SceneDetectorListenerCfg &sceneDetectorListener)
	  { _sceneDetectorListenerCfg = sceneDetectorListener;}

  private:
    void toJson(pt::ptree *root) const;

    Cfgs::VideoStreamCfg _videoStreamCfg;
    VideoStreamCfgJsonify _videoStreamCfgJsonify;
    VideoStreamFactory _videoStreamFactory;
    
    Cfgs::SceneDetectorCfg _sceneDetectorCfg;
    SceneDetectorCfgJsonify _sceneDetectorCfgJsonify;
    
    
    Cfgs::SceneDetectorListenerCfg _sceneDetectorListenerCfg;
    SceneDetectorListenerCfgJsonify _sceneDetectorListenerCfgJsonify;
    SceneDetectorListenerFactory _sceneDetectorListenerFactory;
  };
  
  struct DefaultCfgs{
    static void create(const std::string &filenamePrefix);
  };
}

#endif // SRC_CONFIG_APP_CONFIG_HPP_
