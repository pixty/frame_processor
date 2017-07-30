/*
 * helpers.hpp
 *
 *  Created on: Jun 14, 2017
 *      Author: dmitry
 */

#ifndef SRC_FRAME_PROCESSING_HELPERS_HPP_
#define SRC_FRAME_PROCESSING_HELPERS_HPP_

#include <opencv2/highgui/highgui.hpp>
#include <dlib/opencv.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <opencv2/imgproc.hpp>

namespace fproc {

void draw_polyline(cv::Mat &img, const dlib::full_object_detection& d, const cv::Scalar& color, int start, int end, bool isClosed);
void render_face (cv::Mat &img, const dlib::full_object_detection& d, const cv::Scalar& color);
void rounded_rectangle(cv::Mat& src, cv::Rect rect, const cv::Scalar lineColor, int thickness, int lineType, int cornerRadius);

}
#endif /* SRC_FRAME_PROCESSING_HELPERS_HPP_ */
