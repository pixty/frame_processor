#ifndef SRC_FRAME_PROCESSING1_MULTI_TRACKER_HPP_
#define SRC_FRAME_PROCESSING1_MULTI_TRACKER_HPP_

#include <opencv2/tracking.hpp>

#include "../frame_processing1/algorithms.hpp"
#include "../frame_processing1/box_overlap.hpp"

namespace fproc {

class Tracker{
  public:
    Tracker(const FaceId faceId, const BoxOverlap &tester);
    virtual ~Tracker(){};
    bool init(const CvYMat &frame, const CvRoi &roi);
    const int update(const CvYMat &frame);
    const bool correct(const CvYMat &frame, const CvRoi &roi);
    const int noFaceDetected();
    
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
    cv::Ptr<cv::Tracker> createTracker() const;
    cv::String _trackerName;
    
    const FaceId _faceId;
    const BoxOverlap &_tester;
    double _thresh2Overlap;
    
    cv::Ptr<cv::Tracker> _cvTracker;
    CvRoi _roi;
    int _tmissesCnt;
    int _fmissesCnt;    
};
  
class MultiTracker
{
  typedef std::shared_ptr<Tracker> PTracker;
  public:
    MultiTracker(const int maxFramesToLoose, const BoxOverlap &tester);
    ~MultiTracker();
    
    void start(const CvYMat &frame, 
	       const FaceRegionsList &faces, 
	       FaceRegionsList *detectedAndStarted, 
	       FaceRegionsList *detectedAndNotStarted);
    void track(const CvYMat & frame);
    void update(const CvYMat &frame, const FaceRegionsList &faces, FaceIdsList *lostFaces);
    
    void faceRegions(FaceRegionsList *out) const;
    
  private:
    void sweep(FaceIdsList *lostFaces);
    
    const BoxOverlap &_tester;
    const int _maxFramesToLoose;
    std::list<PTracker> _trackers;
};

}
#endif // SRC_FRAME_PROCESSING1_MULTI_TRACKER_HPP_
