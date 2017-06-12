#ifndef SRC_FRAME_PROCESSING1_BOX_OVERLAP_HPP_
#define SRC_FRAME_PROCESSING1_BOX_OVERLAP_HPP_

#include <opencv2/core/types.hpp>
#include <dlib/image_processing/box_overlap_testing.h>

#include "../frame_processing1/algorithms.hpp"
#include "../model.hpp"

namespace fproc {
  class BoxOverlap
  {
    public:
      BoxOverlap(const double centroidThresh = 0.9, const double areaThresh = 0.99999);

      double centroidOverlap(const CvRoi face, const CvRoi tracker) const;

      double overlap(const CvRoi face, const CvRoi tracker) const;

      void separate(const CvRois &detected, 
		    const FaceRegionsList &tracked, 
		    CvRois *detectedAndNotTracked,
		    FaceRegionsList *detectedAndTracked) const;
		    
      double areaThresh() const {return _areaThresh;}
      double centroidThresh() const {return _centroidThresh;}
    private:      
      double _centroidThresh; 
      double _areaThresh;
  };

  template<typename T>
  inline T min(T v1, T v2){ return v1 < v2 ? v1 : v2;}

  template<typename T>
  inline T max(T v1, T v2){ return v1 > v2 ? v1 : v2;}

  inline const Rectangle cvRoi_to_rectangle(const CvRoi &cvRoi){
    return Rectangle(cvRoi.tl().x, cvRoi.tl().y, cvRoi.br().x-1, cvRoi.br().y-1);	
  }

  inline const CvRoi rectangle_to_cvRoi(const Rectangle &r){
    return CvRoi(r.left(), r.top(), r.width(), r.height());
  }

}
#endif // SRC_FRAME_PROCESSING1_BOX_OVERLAP_HPP_
