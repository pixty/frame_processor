#ifndef SRC_FRAME_PROCESSING1_ALGORITHMS_HPP_
#define SRC_FRAME_PROCESSING1_ALGORITHMS_HPP_

#include "../model.hpp"

namespace fproc {
	/*
	 * Different representations of a frame data
	 */
  	typedef cv::Mat CvYMat;
	typedef dlib::cv_image<unsigned char> DlibYImg;
	
	/*
	 * The Roi represents a region of interest in OpenCV format
	 */
	typedef cv::Rect CvRoi;
	typedef std::vector<CvRoi> CvRois;

	/*
	 * The ForegroundObjectsDetector detects foreground objects in a frame and returns their regions in the frame
	 */
	struct ForegroundObjectsDetector {
		virtual ~ForegroundObjectsDetector() {}
		virtual void detect(const CvYMat &frame, CvRois *objects) = 0;
	};
	

	/*
	 * The FaceDetector detects faces in a frame using the provided regions of interest and returns their regions in the frame
	 */	
	struct FacesDetector{
	  virtual ~FacesDetector(){}
	  virtual void detect(const DlibYImg &frame, const CvRois &candidates, CvRois *final_dets)=0;
	  virtual void detect(const DlibYImg &frame, CvRois *final_dets)=0;
	};
	
	struct FaceRegion{
	  FaceRegion(const FaceId &id, const CvRoi &roi): _id(id), _roi(roi) {}
	  const FaceId& id() const {return _id;}
	  const CvRoi& roi() const {return _roi;}
	private:
	  const FaceId _id;
	  const CvRoi _roi;
	};
	
	typedef std::list<FaceId> FaceIdsList;	
	typedef std::list<FaceRegion> FaceRegionsList;
	
}
#endif // SRC_FRAME_PROCESSING1_ALGORITHMS_HPP_
