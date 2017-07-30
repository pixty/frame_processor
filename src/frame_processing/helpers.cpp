/*
 * helpers.cpp
 *
 *  Created on: Jun 14, 2017
 *      Author: dmitry
 */

#include "helpers.hpp"

namespace fproc {

void draw_polyline(cv::Mat &img, const dlib::full_object_detection& d, const cv::Scalar& color, int start, int end, bool isClosed = false) {
    std::vector <cv::Point> points;
    for (int i = start; i <= end; ++i)
    {
        points.push_back(cv::Point(d.part(i).x(), d.part(i).y()));
    }
    cv::polylines(img, points, isClosed, color, 1, 16);
}

void render_face (cv::Mat &img, const dlib::full_object_detection& d, const cv::Scalar& color) {
    draw_polyline(img, d, color, 0, 16);           // Jaw line
    draw_polyline(img, d, color, 17, 21);          // Left eyebrow
    draw_polyline(img, d, color, 22, 26);          // Right eyebrow
    draw_polyline(img, d, color, 27, 30);          // Nose bridge
    draw_polyline(img, d, 30, 35, true);    // Lower nose
    draw_polyline(img, d, 36, 41, true);    // Left eye
    draw_polyline(img, d, 42, 47, true);    // Right Eye
    draw_polyline(img, d, 48, 59, true);    // Outer lip
    draw_polyline(img, d, 60, 67, true);    // Inner lip

}

void rounded_rectangle(cv::Mat& src, cv::Rect rect, const cv::Scalar lineColor, int thickness, int lineType, int cornerRadius) {
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



