#ifndef SRC_FRAME_PROCESSING_HOG_FACE_DETECTOR_HPP_
#define SRC_FRAME_PROCESSING_HOG_FACE_DETECTOR_HPP_

#include <dlib/image_processing/frontal_face_detector.h>

#include "algorithms.hpp"

namespace fproc {

  class HogFaceDetector : public FacesDetector
  {
    public:
      HogFaceDetector();
      ~HogFaceDetector();
      void detect(const DlibYImg &frame, const CvRois &candidates, CvRois *final_dets);
      void detect(const DlibYImg &frame, CvRois *final_dets);
    private:
      dlib::frontal_face_detector _detector;
  };
}
#endif // SRC_FRAME_PROCESSING_HOG_FACE_DETECTOR_HPP_
