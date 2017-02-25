#ifndef SRC_FRAME_PROCESSING_MULTI_TRACKER_HPP_
#define SRC_FRAME_PROCESSING_MULTI_TRACKER_HPP_

#include <opencv2/tracking.hpp>

#include "algorithms.hpp"

namespace fproc {

class Tracker{
  public:
    Tracker(const FaceId faceId);
    virtual ~Tracker(){};
    bool init(const CvYMat &frame, const CvRoi &roi);
    const int update(const CvYMat &frame);
    const int update(const bool faceDetected, const CvRoi &roi);
    
    const FaceId getFaceId()const{
      return _faceId;
    }
    
    CvRoi getRoi()const{
      return _roi;
    }
    
    const int getTrackMissesCnt()const{
      return _tmissesCnt;
    }
    
    const int getFaceMissesCnt()const{
      return _fmissesCnt;
    }
  private:
    const FaceId _faceId;
    cv::Ptr<cv::Tracker> _cvTracker;
    CvRoi _roi;
    int _tmissesCnt;
    int _fmissesCnt;    
};
  
class MultiTracker
{
  typedef std::shared_ptr<Tracker> PTracker;
  public:
    MultiTracker(const int maxFramesToLoose);
    ~MultiTracker();
    
    void start(const CvYMat &frame, 
	       const FaceRegionsList &faces, 
	       FaceRegionsList *detectedAndStarted, 
	       FaceRegionsList *detectedAndNotStarted);
    void track(const CvYMat & frame);
    void update(const FaceRegionsList &faces, FaceIdsList *lostFaces);
    
    void faceRegions(FaceRegionsList *out) const;
    
  private:
    void sweep(FaceIdsList *lostFaces);
    const int _maxFramesToLoose;
    std::list<PTracker> _trackers;
};

}
#endif // SRC_FRAME_PROCESSING_MULTI_TRACKER_HPP_
