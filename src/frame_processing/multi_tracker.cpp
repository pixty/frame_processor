#include "multi_tracker.hpp"
#include "../logger.hpp"

namespace fproc {

Tracker::Tracker(const FaceId faceId, const BoxOverlap &tester):
  _faceId(faceId),
  _tester(tester),
  _thresh2Overlap(-1),
  _trackerName("MEDIANFLOW"),
  _tmissesCnt(0),
  _fmissesCnt(0)
{
  _cvTracker = createTracker();
}

cv::Ptr<cv::Tracker> Tracker::createTracker() const{
  return cv::Tracker::create(_trackerName);
}

bool Tracker::init(const CvYMat &frame, const CvRoi &roi){
    if(!_cvTracker->init(frame, roi)){
      _tmissesCnt++;
    }
    _roi = roi;
    return _tmissesCnt == 0;
}

const int Tracker::update(const CvYMat &frame){
  cv::Rect2d roi;
  if(!_cvTracker->update(frame, roi)){
    _tmissesCnt++;
  }else{
    _tmissesCnt = 0;
    _roi=roi;
  }
  return _tmissesCnt;
}

const bool Tracker::correct(const CvYMat &frame, const CvRoi &roi){
  _fmissesCnt = 0;
  double overlap = _tester.overlap(roi, _roi);
  double threshold = _tester.areaThresh();
  double thresh2Overlap = threshold/overlap;
  bool initialized = true;
  _roi = roi;
  if( _thresh2Overlap > 0 
      && (thresh2Overlap > _thresh2Overlap /* going worsen */
      && thresh2Overlap + (thresh2Overlap - _thresh2Overlap) > 0.99))
  {
    LOG_INFO("Going to reinitialize " << thresh2Overlap << "=curr, prev=" << _thresh2Overlap);
    cv::Ptr<cv::Tracker> oldTracker = _cvTracker;
    _cvTracker = createTracker();
    initialized =  init(frame, roi);
    if(!initialized){
      LOG_INFO("Reinitialization is failed");
      _cvTracker = oldTracker;
    }else{
      LOG_INFO("a new one tracker");
    }
  }
  _thresh2Overlap = thresh2Overlap;
  return initialized;
}

const int Tracker::noFaceDetected(){
  return ++_fmissesCnt;
}

MultiTracker::MultiTracker(const int maxFramesToLoose, const BoxOverlap &tester):
  _maxFramesToLoose(maxFramesToLoose),
  _tester(tester)
{

}

MultiTracker::~MultiTracker()
{
}

void MultiTracker::start(const CvYMat &frame, 
		         const FaceRegionsList &faces,
			 FaceRegionsList *detectedAndStarted,
			 FaceRegionsList *detectedAndNotStarted) {
  detectedAndStarted->clear();
  for(FaceRegion face : faces){    
    const PTracker t (new Tracker(face.id(), _tester));
    if(t->init(frame, face.roi())){
      _trackers.push_back(t);
      detectedAndStarted->push_back(face);
    }else{
      detectedAndNotStarted->push_back(face);
    }
  }
}

void MultiTracker::track(const CvYMat &frame){
  std::list<PTracker>::iterator itr = _trackers.begin();
  std::list<PTracker>::iterator end = _trackers.end();
  while(itr != end){
    (*itr)->update(frame);
    itr++;
  }
}

void MultiTracker::update(const CvYMat &frame, const FaceRegionsList &faceRegions, FaceIdsList *lostFaces){  
  for(PTracker t : _trackers){
    FaceRegion *fr = nullptr;
    for(FaceRegion candidate : faceRegions){
      bool faceDetected = false;
      if(candidate.id() == t->getFaceId()){	
	fr = &candidate;
	break;
      } 
    }
    if(fr != nullptr){
      t->correct(frame, fr->roi());
    }else{
      t->noFaceDetected();
    }
  }
  sweep(lostFaces);
}

void MultiTracker::sweep(FaceIdsList *lostFaces){
  lostFaces->clear();
  std::list<PTracker>::iterator itr = _trackers.begin();
  while(itr != _trackers.end()){
    // TODO what shall we do if threre is no face for X frames in a tracker?
    if((*itr)->getTrackMissesCnt() < _maxFramesToLoose /*|| (*itr)->getFaceMissesCnt() < _maxFramesToLoose*/){
      itr++;
    } else {
      lostFaces->push_back((*itr)->getFaceId());
      itr = _trackers.erase(itr);
    }
  }  
}

void MultiTracker::faceRegions(FaceRegionsList *out) const{
  out->clear();
  for(PTracker t : _trackers){
    out->push_back(FaceRegion(t->getFaceId(), t->getRoi()));
  }
}
    
}
