#include "naive_scene_detector.hpp"

#include "grayscaler.hpp"
#include "haar_face_detector.hpp"
#include "hog_face_detector.hpp"
#include "../logger.hpp"

namespace fproc {
  
NaiveSceneDetector::NaiveSceneDetector(
	VideoStream& vstream, 
	SceneDetectorListener& listener,
	PDebugger &debugger,
	const NaiveSceneDetectorParameters &parameters):
		SceneDetector(vstream, listener),
		_debugger(std::move(debugger)),
		_box_overlap(parameters._iou_thresh, 
			     parameters._percent_covered_thresh),
		_maxFaces(parameters._maxFaces),
		_multiTracker(parameters._maxFramesToLooseTrack),
		_foreground_det(new HaarFaceDetector(
					parameters._minFaceSize, 
					parameters._maxFaceSize)),
		_face_det(new HogFaceDetector()){
}

NaiveSceneDetector::~NaiveSceneDetector()
{
}

void NaiveSceneDetector::doProcess(PFrame frame){
	(*_debugger)();
	// 1. transform the frame to a grayscale image
	Grayscaler::transform(frame->get_mat(), &_grayedFrame);
	
	// 2. run the trackers
	_multiTracker.track(_grayedFrame);
	
	// 3. run the detectors
	CvRois detectedFaces;	
	detectFaces(&detectedFaces);
	
	// 4. merge the detections and the trackers
	FaceRegionsList detectedAndNotTracked;
	FaceRegionsList detectedAndTracked;
	FaceRegionsList trackedAtTheMoment;
	_multiTracker.faceRegions(&trackedAtTheMoment);
	parseDetectedFaces(detectedFaces, trackedAtTheMoment, 
			   &detectedAndNotTracked, &detectedAndTracked);
	
	// 5. start new trackers if new faces are detected
	FaceRegionsList detectedAndStarted;
	FaceRegionsList detectedAndNotStarted;
	_multiTracker.start(_grayedFrame, detectedAndNotTracked, &detectedAndStarted, &detectedAndNotStarted);
	
	// 6. correct trackers with the best knowledge about the faces
	// Disclaimer: Unfortunately the opencv trackers can not be corrected so just update the stored regions
	FaceIdsList lostFaces;
	_multiTracker.update(detectedAndTracked, &lostFaces);
	
	// 7. push the results to the scene
	FaceRegionsList allTracked;
	_multiTracker.faceRegions(&allTracked);
	updateScene(frame, allTracked, detectedAndStarted, detectedAndNotStarted, detectedAndTracked, lostFaces);
	// 8. debug
	(*_debugger)(*this, frame, allTracked, detectedAndStarted, detectedAndNotStarted, detectedAndTracked, lostFaces);	
}

void NaiveSceneDetector::parseDetectedFaces(const CvRois &detected,
					    const FaceRegionsList &tracked, 
					    FaceRegionsList  *detectedAndNotTracked, 
					    FaceRegionsList  *detectedAndTracked)
{
	// find new objects in the final_dets which are not in the multi tracker
	CvRois detectedAndNotTrackedRois;
	_box_overlap.separate(detected, tracked, 
			      &detectedAndNotTrackedRois, detectedAndTracked);	
	createFaceRegions(detectedAndNotTrackedRois, detectedAndNotTracked);
}

void NaiveSceneDetector::createFaceRegions(const CvRois &src, FaceRegionsList *dest){
  dest->clear();
  for(const CvRoi roi : src){
    const FaceId faceId = _uuid_generator.get();
    dest->push_back(FaceRegion(faceId, roi));
  }  
}

void NaiveSceneDetector::detectFaces(CvRois *detectedFaces){
	// detect foreground objects
	CvRois candidates;
	_foreground_det->detect(_grayedFrame, &candidates);	
	// find faces
	_face_det->detect(DlibYImg(_grayedFrame), candidates, detectedFaces);
}

void NaiveSceneDetector::updateScene(
  const PFrame &frame,
  const FaceRegionsList &tracked,
  const FaceRegionsList &detectedAndStarted,
  const FaceRegionsList &detectedAndNotStarted,
  const FaceRegionsList &detectedAndTracked,
  const FaceIdsList &lostFaces){
  // TODO implement me
  if(detectedAndStarted.size() > 0 || lostFaces.size() > 0){
    // creane a new scene
    // Scene *scene = new Scene(frame->getTimestamp());
  }else{
    // update current scene
  }
  // update scene
  if(_scene.get() != nullptr){
    // move
  }
  /*
	  struct Face {
	  std::list<FrameRegion> getImages();
	  FaceId getId();
	  Timestamp firstTimeCatched();

	  /* Other methods and members are not defined yet 
  };*/  
}

void NaiveSceneDetector::onStop(){
  
}

}
