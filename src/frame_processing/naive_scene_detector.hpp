#ifndef SRC_FRAME_PROCESSING_NAIVE_SCENE_DETECTOR_HPP_
#define SRC_FRAME_PROCESSING_NAIVE_SCENE_DETECTOR_HPP_

#include "algorithms.hpp"
#include "multi_tracker.hpp"
#include "box_overlap.hpp"
#include "uuid.hpp"

namespace fproc {
  
// The parameters of the Naive Scene Detector  
struct NaiveSceneDetectorParameters{  
  NaiveSceneDetectorParameters(
	const int minFaceSize = 20,
	const int maxFaceSize = 300,
	const int maxFramesToLooseTrack = 10,
	const double iou_thresh = 0.8, 
	const double percent_covered_thresh = 0.8,
	const int maxFaces = 5):
	  _minFaceSize(minFaceSize), 
	  _maxFaceSize(maxFaceSize),
	  _maxFramesToLooseTrack(maxFramesToLooseTrack), 
	  _iou_thresh(iou_thresh),
	  _percent_covered_thresh(percent_covered_thresh),
	  _maxFaces(maxFaces)
	{};
	
  const int minFaceSize() const {return _minFaceSize;}
  const int maxFaceSize() const {return _maxFaceSize;}
  const int maxFramesToLooseTrack() const {return _maxFramesToLooseTrack;}
  // boxes intersections thresholds
  const double iou_thresh() const {return _iou_thresh;}
  const double percent_covered_thresh() const {return _percent_covered_thresh;}
  // TODO ignore the smallest faces if total faces is more than maxFaces  
  const int maxFaces() const {return _maxFaces;}
  
  void minFaceSize(int minFaceSize) {_minFaceSize = minFaceSize;}
  void maxFaceSize(int maxFaceSize) {_maxFaceSize = maxFaceSize;}
  void maxFramesToLooseTrack(int maxFramesToLooseTrack) {_maxFramesToLooseTrack = maxFramesToLooseTrack;}
  void iou_thresh(double iou_thresh) {_iou_thresh = iou_thresh;}
  void percent_covered_thresh(double percent_covered_thresh) {_percent_covered_thresh = percent_covered_thresh;}
  void maxFaces(int maxFaces) {_maxFaces = maxFaces;}

private:
  int _minFaceSize;
  int _maxFaceSize;
  int _maxFramesToLooseTrack;
  double _iou_thresh; 
  double _percent_covered_thresh;
  int _maxFaces;
};


class NaiveSceneDetector : public SceneDetector
{
public:
  struct Debugger{
    virtual ~Debugger(){}
    virtual void operator()() {};
    virtual void operator() (
	    const NaiveSceneDetector &detectotr,
	    const PFrame &frame,
	    const FaceRegionsList &tracked,
	    const FaceRegionsList &detectedAndStarted,
	    const FaceRegionsList &detectedAndNotStarted,
	    const FaceRegionsList &detectedAndTracked,
	    const FaceIdsList &lostFaces
	  ){};
  };

  typedef std::unique_ptr<Debugger> PDebugger;
  
  NaiveSceneDetector(PVideoStream vstream, 
		     PSceneDetectorListener listener,
		     PDebugger debugger,
		     const NaiveSceneDetectorParameters &parameters = NaiveSceneDetectorParameters());
  ~NaiveSceneDetector();
	
protected:
  void doProcess(PFrame frame);
  void onStop();
	
private:
  void detectFaces(CvRois *detectedFaces);
  void parseDetectedFaces(const CvRois &detected, const FaceRegionsList &tracked,
			  FaceRegionsList *detectedAndNotTracked, FaceRegionsList * detectedAndTracked);
  // helper functions
  void createFaceRegions(const CvRois &src, FaceRegionsList * dest);
  void updateScene(const PFrame &frame,
		   const FaceRegionsList &tracked,
		   const FaceRegionsList &detectedAndStarted,
		   const FaceRegionsList &detectedAndNotStarted,
		   const FaceRegionsList &detectedAndTracked,
		   const FaceIdsList &lostFaces);
  void addFacesList(const PFrame &frame, const FaceRegionsList &faceRegions, PFList *faces);
  void removeFacesList(const FaceIdsList &lostFaces, PFList *faces);
  void updateFacesList(const PFrame &frame, const FaceIdsList &lostFaces, PFList *faces);
  void updateFacesList(const PFrame &frame, const FaceRegionsList &faceRegions, PFList *faces);
  PFace getFace(const FaceId &id, const PFList &faces);
  CvYMat _grayedFrame;
  const int _maxFaces;
  std::unique_ptr<ForegroundObjectsDetector> _foreground_det;
  std::unique_ptr<FacesDetector> _face_det;
  MultiTracker _multiTracker;
  BoxOverlap _box_overlap;
  Uuid _uuid_generator;
  
  // Debugger
  PDebugger _debugger;
};

}
#endif // SRC_FRAME_PROCESSING_NAIVE_SCENE_DETECTOR_HPP_
