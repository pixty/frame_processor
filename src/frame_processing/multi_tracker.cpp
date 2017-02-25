#include "multi_tracker.hpp"

namespace fproc {

Tracker::Tracker(const FaceId faceId):
  _faceId(faceId),
  _cvTracker(cv::Tracker::create("MEDIANFLOW")),
  _tmissesCnt(0),
  _fmissesCnt(0)
{
  
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

const int Tracker::update(const bool faceDetected, const CvRoi &roi){
  if(faceDetected){
    _fmissesCnt = 0;
    _roi = roi;
  }else{
    _fmissesCnt++;
  }
  return _fmissesCnt;
}

MultiTracker::MultiTracker(const int maxFramesToLoose):
  _maxFramesToLoose(maxFramesToLoose)
{

}

MultiTracker::~MultiTracker()
{
  std::list<PTracker>::iterator itr = _trackers.begin();
  // TODO fix me
  while(itr != _trackers.end()){
    itr = _trackers.erase(itr);
  }
}

void MultiTracker::start(const CvYMat &frame, 
		         const FaceRegionsList &faces,
			 FaceRegionsList *detectedAndStarted,
			 FaceRegionsList *detectedAndNotStarted) {
  detectedAndStarted->clear();
  for(FaceRegion face : faces){    
    const PTracker t (new Tracker(face.id()));
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

void MultiTracker::update(const FaceRegionsList &faceRegions, FaceIdsList *lostFaces){  
  for(PTracker t : _trackers){    
    for(FaceRegion candidate : faceRegions){
      bool faceDetected = false;
      if(candidate.id() == t->getFaceId()){	
	faceDetected = true;
	break;
      }
      t->update(faceDetected, CvRoi());
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
    // TODO fix missed trackers    
    out->push_back(FaceRegion(t->getFaceId(), t->getRoi()));
  }
}
    
}
