#ifndef SRC_FRAME_PROCESSING_BOX_OVERLAP_HPP_
#define SRC_FRAME_PROCESSING_BOX_OVERLAP_HPP_

#include <opencv2/core/types.hpp>
#include <dlib/image_processing/box_overlap_testing.h>
#include "../model.hpp"
#include "algorithms.hpp"

namespace fproc {
  class BoxOverlap
  {
    public:
      BoxOverlap(double iou_thresh_ = 0.9, double percent_covered_thresh_ = 0.99999);
      void separate(const CvRois &detected, 
		    const FaceRegionsList &tracked, 
		    CvRois *detectedAndNotTracked,
		    FaceRegionsList *detectedAndTracked) const;
    private:      
      dlib::test_box_overlap _tester;
  };
  
  inline const Rectangle cvRoi_to_rectangle(const CvRoi &cvRoi){
    return Rectangle(cvRoi.tl().x, cvRoi.tl().y, cvRoi.br().x-1, cvRoi.br().y-1);	
  }
      
  inline const CvRoi rectangle_to_cvRoi(const Rectangle &r){
    return CvRoi(r.left(), r.top(), r.width(), r.height());
  }
  
}
#endif // SRC_FRAME_PROCESSING_BOX_OVERLAP_HPP_
