#ifndef SRC_FRAME_PROCESSING1_GRAYSCALER_HPP_
#define SRC_FRAME_PROCESSING1_GRAYSCALER_HPP_

#include "../frame_processing1/algorithms.hpp"

namespace fproc {
  struct Grayscaler{    
    static void transform(Frame::CvBgrMat & source, CvYMat *dest){
	cv::cvtColor(source, *dest, CV_BGR2GRAY);
    }
  };
}
#endif // SRC_FRAME_PROCESSING1_GRAYSCALER_HPP_
