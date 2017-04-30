#ifndef SRC_FRAME_PROCESSING_NAIVE_SCENE_DETECTOR_DEBUGGER_HPP
#define SRC_FRAME_PROCESSING_NAIVE_SCENE_DETECTOR_DEBUGGER_HPP

#include <dlib/gui_widgets.h>

#include "naive_scene_detector.hpp"
#include "../simple_profiler.hpp"

namespace fproc {

class NaiveSceneDetectorDebugger: public NaiveSceneDetector::Debugger{
public:
NaiveSceneDetectorDebugger();
~NaiveSceneDetectorDebugger();

void operator()();

void operator()(
  const NaiveSceneDetector &detector,
  const PFrame &frame,
  const FaceRegionsList &tracked,  
  const FaceRegionsList &detectedAndStarted,
  const FaceRegionsList &detectedAndNotStarted,
  const FaceRegionsList &detectedAndTracked,
  const FaceIdsList &lostFaces
);

private:
  void updatePerformanceMetrics();
  void getRects(const FaceRegionsList &src, std::vector<Rectangle> *out) const;
  const std::string coma_separated(const FaceIdsList &faceIds) const;
  const int avgFps() const;
  
  dlib::image_window _win;
  // Performance metrics
  SimpleProfiler _profiler;
  uint64 _minFrameProcessingTime;
  uint64 _maxFrameProcessingTime;
  uint64 _totalProcessingTime;
  int _framesCnt;
};

}
#endif // SRC_FRAME_PROCESSING_NAIVE_SCENE_DETECTOR_DEBUGGER_HPP
