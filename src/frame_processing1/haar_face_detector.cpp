#include "../frame_processing1/haar_face_detector.hpp"

#include <opencv2/core/cvstd.hpp>

#include "../frame_processing1/haarcascade_frontalface_default_xml.hpp"

namespace fproc {

HaarFaceDetector::HaarFaceDetector(const int minFaceSize, const int maxFaceSize):
  _minFaceSize(minFaceSize, minFaceSize),
  _maxFaceSize(maxFaceSize, maxFaceSize)
{
  cv::FileStorage fs( haarcascade_frontalface_default_xml, cv::FileStorage::READ | cv::FileStorage::MEMORY);
  _cvFaceDetector.read(fs.getFirstTopLevelNode());
}

HaarFaceDetector::~HaarFaceDetector(){
}

void HaarFaceDetector::detect(const CvYMat &frame, CvRois *objects){
  objects->clear();
  if(_maxFaceSize.height <= 0 || _minFaceSize.height <= 0){
    _cvFaceDetector.detectMultiScale(frame, *objects, 1.3);
  }else{
    _cvFaceDetector.detectMultiScale(frame, *objects, 1.3, 3, 0, _minFaceSize, _maxFaceSize);
  }
}

}
