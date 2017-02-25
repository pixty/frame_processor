#ifndef SRC_FRAME_PROCESSING_HAAR_FACE_DETECTOR_HPP_
#define SRC_FRAME_PROCESSING_HAAR_FACE_DETECTOR_HPP_

#include <opencv2/objdetect.hpp>

#include "algorithms.hpp"

namespace fproc {

class HaarFaceDetector :  public ForegroundObjectsDetector
{
public:
  HaarFaceDetector(const int minFaceSize, const int maxFaceSize);
  HaarFaceDetector(const char * model, const int minFaceSize=0, const int maxFaceSize = 0);
  ~HaarFaceDetector();
  void detect(const CvYMat &frame, CvRois *objects);
private:
  cv::CascadeClassifier _cvFaceDetector;
  cv::Size _minFaceSize;
  cv::Size _maxFaceSize;
};

}
#endif // SRC_FRAME_PROCESSING_HAAR_FACE_DETECTOR_HPP_
