#include "hog_face_detector.hpp"
#include "box_overlap.hpp"

namespace fproc {
  
HogFaceDetector::HogFaceDetector():
  _detector(dlib::get_frontal_face_detector())
{

}

HogFaceDetector::~HogFaceDetector()
{

}

void HogFaceDetector::detect(const DlibYImg &frame, CvRois *final_dets){
    // TODO confidence
    std::vector<Rectangle> tmp_final_dets;
    tmp_final_dets = _detector(frame);
    final_dets->clear();
    for(Rectangle r : tmp_final_dets){
      final_dets->push_back(rectangle_to_cvRoi(r));
    }
}

void HogFaceDetector::detect(const DlibYImg &frame, const CvRois &candidates, CvRois *final_dets){
  final_dets->clear();
  for(CvRoi bb : candidates){
    Rectangle orig = cvRoi_to_rectangle(bb);
    std::vector<Rectangle> faces = _detector(dlib::sub_image(frame, orig));
    if(faces.size() > 0){
      // naive approach: just pick the first one up
      CvRoi dbb = rectangle_to_cvRoi(dlib::translate_rect(faces[0], orig.left(), orig.top()));
      final_dets->push_back(dbb);
    }
  }  
}

}