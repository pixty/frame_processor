#include "naive_scene_detector_debugger.hpp"
#include "algorithms.hpp"
#include "../logger.hpp"

namespace fproc {
  
NaiveSceneDetectorDebugger::NaiveSceneDetectorDebugger():
    _profiler("NaiveSceneDetectorDebugger"),
    _minFrameProcessingTime(0),
    _maxFrameProcessingTime(0),
    _totalProcessingTime(0),
    _framesCnt(0)
{

}

NaiveSceneDetectorDebugger::~NaiveSceneDetectorDebugger()
{
  if(!_win.is_closed()){
    _win.close_window();    
  }
  LOG_INFO("Total frames processed:");
  LOG_INFO(_framesCnt);
  LOG_INFO("Total processing time (s):");
  LOG_INFO(_totalProcessingTime/(1000.0*1000.0));
  LOG_INFO("FPS:");  
  LOG_INFO(avgFps());
  LOG_INFO("Max frame processessing time (ms):");
  LOG_INFO(_maxFrameProcessingTime/1000.0);
  LOG_INFO("Min frame processessing time (ms):");
  LOG_INFO(_minFrameProcessingTime/1000.0);
}

static dlib::rgb_pixel red(255,0,0);
static dlib::rgb_pixel green(0,255,0);
static dlib::rgb_pixel blue(0,0,255);
static dlib::rgb_pixel black(0,0,0);
static dlib::rgb_pixel white(255,255,255);


void NaiveSceneDetectorDebugger::operator()(){
  _profiler.enter();
}

void NaiveSceneDetectorDebugger::operator()(
  const NaiveSceneDetector &detector,
  const PFrame &frame, 
  const FaceRegionsList &tracked,
  const FaceRegionsList &detectedAndStarted,
  const FaceRegionsList &detectedAndNotStarted,
  const FaceRegionsList &detectedAndTracked,
  const FaceIdsList &lostFaces){
  // profiler stuff
  _profiler.exit();
  updatePerformanceMetrics();
  // check the window
  if(_win.is_closed()){
    // hack
    const_cast<NaiveSceneDetector*>(&detector)->stop(); 
    return;
  }
  // draw the stuff
  _win.set_image(frame->get_cv_image());
  _win.clear_overlay();
  std::vector<Rectangle> out;
  getRects(detectedAndStarted, &out);
  _win.add_overlay(out, red);
  getRects(detectedAndTracked, &out);
  _win.add_overlay(out, green);  
  getRects(tracked, &out);
  _win.add_overlay(out, blue);
  std::ostringstream ostr;  
  ostr << "FPS: " << avgFps() << "\nLost: " << coma_separated(lostFaces) << "\n";
  _win.add_overlay(Rectangle(0,0,0,0), black, ostr.str());
  // print the stuff
  if(detectedAndNotStarted.size() > 0 || tracked.size() > 1 || detectedAndStarted.size() > 0){
    if(tracked.size() > 1){	
      LOG_INFO("tracked.size" << tracked.size() << " {");
      FaceRegionsList::const_iterator itr = tracked.begin();
      LOG_INFO((itr->id()) << "-:-" << (itr->roi()));
      itr++;
      LOG_INFO((itr->id()) << "-:-" << (itr->roi()));
      LOG_INFO("} tracked.size");
    }
    if(detectedAndStarted.size() > 0){
      LOG_INFO("!detectedAndStarted {");
      FaceRegionsList::const_iterator itr = detectedAndStarted.begin();
      LOG_INFO(itr->id() << itr->roi());
      LOG_INFO("} !detectedAndStarted");
    }
    if(detectedAndNotStarted.size() > 0){
      LOG_INFO("!detectedAndNotStarted" << detectedAndNotStarted.begin()->roi());
    }
    if(detectedAndTracked.size() > 0){
      LOG_INFO("!detectedAndTracked" << detectedAndTracked.begin()->roi());
    }
  }	
  if(lostFaces.size() > 0){
    LOG_INFO("!lostFaces:   " << *lostFaces.begin());
  }
}

void NaiveSceneDetectorDebugger::updatePerformanceMetrics(){
    uint64 totalProcessingTime = _profiler.time();
    uint64 current = totalProcessingTime - _totalProcessingTime;
    if(current > _maxFrameProcessingTime || _maxFrameProcessingTime <= 0) _maxFrameProcessingTime = current;
    if(current < _minFrameProcessingTime || _minFrameProcessingTime <= 0) _minFrameProcessingTime = current;
    _totalProcessingTime = totalProcessingTime;
    _framesCnt++;
}

void NaiveSceneDetectorDebugger::getRects(const FaceRegionsList &src, std::vector<Rectangle> *out) const{
  out->clear();
  for(FaceRegion fr : src){
    out->push_back(cvRoi_to_rectangle(fr.roi()));
  }
}

const int NaiveSceneDetectorDebugger::avgFps() const{
  return (_framesCnt*1000.0*1000.0)/_totalProcessingTime;
}

const std::string NaiveSceneDetectorDebugger::coma_separated(const FaceIdsList &faceIds) const{
  std::ostringstream ostr;
  for(FaceId id : faceIds){
    ostr << id << ", ";
  }
  return ostr.str();
}

}
