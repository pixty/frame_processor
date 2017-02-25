#include "box_overlap.hpp"

namespace fproc {

BoxOverlap::BoxOverlap(double iou_thresh_, double percent_covered_thresh_):
  _tester(iou_thresh_, percent_covered_thresh_)
{

}

// opencv::overlap {
// 	double a1 = r1.area(), a2 = r2.area(), a0 = (r1&r2).area();
// 	return a0 / (a1 + a2 - a0);
// }

void BoxOverlap::separate(const CvRois &detected, 
			  const FaceRegionsList &tracked, 
			  CvRois *detectedAndNotTracked,
			  FaceRegionsList *detectedAndTracked) const 
{
  detectedAndNotTracked->clear();
  detectedAndTracked->clear();
  for(CvRoi newR : detected){
    bool found = false;
    Rectangle tmpNewR = cvRoi_to_rectangle(newR);
    for(FaceRegion trackedFace: tracked){
	Rectangle tmpOrigR = cvRoi_to_rectangle(trackedFace.roi());	
	if( _tester(tmpNewR, tmpOrigR)){
	  detectedAndTracked->push_back(FaceRegion(trackedFace.id(), newR));
	  found = true;
	  break;
	}
    }
    if(!found){
      detectedAndNotTracked->push_back(newR);
    }
  }
}

}