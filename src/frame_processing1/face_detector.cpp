/*
 * face_detector.cpp
 *
 *  Created on: Jan 31, 2017
 *      Author: dmitry
 */

#include "../frame_processing1/face_detector.hpp"

#include <opencv2/highgui/highgui.hpp>

#include <dlib/opencv.h>
//#include <opencv2/highgui/highgui.hpp>
//#include <dlib/image_processing/render_face_detections.h>
//#include <dlib/image_processing.h>
//#include <dlib/gui_widgets.h>
#include <opencv2/imgproc.hpp>
//#include <opencv2/opencv.hpp>

using namespace dlib;

namespace fproc {

static void render_face (cv::Mat &img, const dlib::full_object_detection& d);
static void rounded_rectangle(cv::Mat& src, cv::Rect rect, const cv::Scalar lineColor, const int thickness, const int lineType , const int cornerRadius);

FaceDetector::FaceDetector(const std::string &faceLandmarksModelFilename) {
	_detector = get_frontal_face_detector();
	deserialize(faceLandmarksModelFilename) >> _pose_model;
}

FRList& FaceDetector::detectRegions(PFrame pFrame) {
	cv::Mat& im = pFrame->get_mat();
	cv_image<bgr_pixel> cimg(im);
	std::vector<dlib::rectangle> faces = _detector(cimg);

	std::vector<full_object_detection> shapes;
	for (unsigned long i = 0; i < faces.size(); ++i)
	{
		// Resize obtained rectangle for full resolution image.
		dlib::rectangle r(
					(long)(faces[i].left()),
					(long)(faces[i].top()),
					(long)(faces[i].right()),
					(long)(faces[i].bottom())
					);
		cv::Rect cv_r((int)(faces[i].left()),
					(int)(faces[i].top()), (int)r.width(), (int)r.height());

		// Landmark detection on full sized image
		full_object_detection shape = _pose_model(cimg, r);
		shapes.push_back(shape);

		// Custom Face Render
		render_face(im, shape);
		//cv::rectangle(im, cv_r, cv::Scalar(255, 255, 0), 1, 16);
		rounded_rectangle(im, cv_r, cv::Scalar(58, 242, 252), 1, 16, 5);
	}

	return _frame_regions;
}

static void draw_polyline(cv::Mat &img, const dlib::full_object_detection& d, const int start, const int end, bool isClosed = false)
{
    std::vector <cv::Point> points;
    for (int i = start; i <= end; ++i)
    {
        points.push_back(cv::Point(d.part(i).x(), d.part(i).y()));
    }
    cv::polylines(img, points, isClosed, cv::Scalar(161, 209, 255), 1, 16);
}

static void render_face (cv::Mat &img, const dlib::full_object_detection& d)
{
    draw_polyline(img, d, 0, 16);           // Jaw line
    draw_polyline(img, d, 17, 21);          // Left eyebrow
    draw_polyline(img, d, 22, 26);          // Right eyebrow
    draw_polyline(img, d, 27, 30);          // Nose bridge
    draw_polyline(img, d, 30, 35, true);    // Lower nose
    draw_polyline(img, d, 36, 41, true);    // Left eye
    draw_polyline(img, d, 42, 47, true);    // Right Eye
    draw_polyline(img, d, 48, 59, true);    // Outer lip
    draw_polyline(img, d, 60, 67, true);    // Inner lip

}

static void rounded_rectangle(cv::Mat& src, cv::Rect rect, const cv::Scalar lineColor, const int thickness, const int lineType , const int cornerRadius)
{
	using namespace cv;
    /* corners:
     * p1 - p2
     * |     |
     * p4 - p3
     */
	cv::Point topLeft(rect.tl());
	cv::Point bottomRight(rect.br());

    Point p1 = topLeft;
    Point p2 = Point (bottomRight.x, topLeft.y);
    Point p3 = bottomRight;
    Point p4 = Point (topLeft.x, bottomRight.y);

    // draw straight lines
    line(src, Point (p1.x+cornerRadius,p1.y), Point (p2.x-cornerRadius,p2.y), lineColor, thickness, lineType);
    line(src, Point (p2.x,p2.y+cornerRadius), Point (p3.x,p3.y-cornerRadius), lineColor, thickness, lineType);
    line(src, Point (p4.x+cornerRadius,p4.y), Point (p3.x-cornerRadius,p3.y), lineColor, thickness, lineType);
    line(src, Point (p1.x,p1.y+cornerRadius), Point (p4.x,p4.y-cornerRadius), lineColor, thickness, lineType);

    // draw arcs
    ellipse( src, p1+Point(cornerRadius, cornerRadius), Size( cornerRadius, cornerRadius ), 180.0, 0, 90, lineColor, thickness, lineType );
    ellipse( src, p2+Point(-cornerRadius, cornerRadius), Size( cornerRadius, cornerRadius ), 270.0, 0, 90, lineColor, thickness, lineType );
    ellipse( src, p3+Point(-cornerRadius, -cornerRadius), Size( cornerRadius, cornerRadius ), 0.0, 0, 90, lineColor, thickness, lineType );
    ellipse( src, p4+Point(cornerRadius, -cornerRadius), Size( cornerRadius, cornerRadius ), 90.0, 0, 90, lineColor, thickness, lineType );
}


}

