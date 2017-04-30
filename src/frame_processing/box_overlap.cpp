#include "box_overlap.hpp"

namespace fproc {

BoxOverlap::BoxOverlap(const double centroidThresh, const double areaThresh):
  _centroidThresh(centroidThresh), 
  _areaThresh(areaThresh)
{

}

double BoxOverlap::overlap(const CvRoi face, const CvRoi tracker) const {
	double a1 = face.area(), a2 = tracker.area(), a0 = (face&tracker).area();
	return a0 / (a1 + a2 - a0); 
}

double BoxOverlap::centroidOverlap(const CvRoi face, const CvRoi tracker) const {
  
  double fw2 = face.width/2;
  double cfx = face.x + fw2;
  double tw2 = tracker.width/2;
  double ctx = tracker.x + tw2;
  double cxe = abs(cfx - ctx) / min(fw2, tw2);
  
  double fh2 = face.height/2;
  double cfy = face.y + fh2;
  double th2 = tracker.height/2;
  double cty = tracker.y + th2;
  double cye = abs(cfy - cty) / min(fh2, th2);
  
  return min(cxe, cye); // <> || ^_ vs  <^ || ^>
}

void BoxOverlap::separate(const CvRois &detected, 
			  const FaceRegionsList &tracked, 
			  CvRois *detectedAndNotTracked,
			  FaceRegionsList *detectedAndTracked) const 
{
  detectedAndNotTracked->clear();
  detectedAndTracked->clear();
  for(CvRoi face : detected){
    
    bool found = false;
    for(FaceRegion trackedFace: tracked){
      double aOvl = overlap(face, trackedFace.roi());
      double cOvl = centroidOverlap(face, trackedFace.roi());
      if( aOvl > _areaThresh ||
	  cOvl < 1.0 - _centroidThresh){
	detectedAndTracked->push_back(FaceRegion(trackedFace.id(), face));
	found = true;
	break;
      }
    }
    if(!found){
      detectedAndNotTracked->push_back(face);
    }
  }
}

}