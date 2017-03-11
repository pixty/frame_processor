/*
 * face_detector.cpp
 *
 *  Created on: Jan 31, 2017
 *      Author: dmitry
 */

#include "face_detector.hpp"
//#include <dlib/image_processing/frontal_face_detector.h>
//#include <dlib/gui_widgets.h>
//#include <dlib/image_io.h>
//#include <iostream>
#include <opencv2/highgui/highgui.hpp>

#include <dlib/opencv.h>
//#include <opencv2/highgui/highgui.hpp>
//#include <dlib/image_processing/render_face_detections.h>
//#include <dlib/image_processing.h>
//#include <dlib/gui_widgets.h>
//#include <opencv2/opencv.hpp>

#include "../logger.hpp"

using namespace dlib;

namespace fproc {

static void render_face (cv::Mat &out, const dlib::full_object_detection& d);
static void rounded_rectangle(cv::Mat& out, cv::Rect rect, const cv::Scalar lineColor, const int thickness, const int lineType , const int cornerRadius);
static void put_text(cv::Mat &out, const std::string str, const int x, const int y,
                 const int fontFace = cv::FONT_HERSHEY_COMPLEX_SMALL, 
                 const double fontScale = 0.8, const cv::Scalar color = cv::Scalar(255,255,255),
                 const int thickness=1, const int lineType=8);

FaceDetector::FaceDetector(const std::string &faceLandmarksModelFilename) {
	_detector = get_frontal_face_detector();
	deserialize(faceLandmarksModelFilename) >> _pose_model;
}

FRList& FaceDetector::detectRegions(PFrame pFrame) {
	cv::Mat& im = pFrame->get_mat();
	cv_image<bgr_pixel> cout(im);
	std::vector<dlib::rectangle> faces = _detector(cout);
    demo(im, pFrame->getId(), faces);
/*
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
		full_object_detection shape = _pose_model(cout, r);
		shapes.push_back(shape);

		// Custom Face Render
		render_face(im, shape);
		//cv::rectangle(im, cv_r, cv::Scalar(255, 255, 0), 1, 16);
		rounded_rectangle(im, cv_r, cv::Scalar(58, 242, 252), 1, 16, 5);
	}
*/
	return _frame_regions;
}

static void draw_polyline(cv::Mat &out, const dlib::full_object_detection& d, const int start, const int end, bool isClosed = false)
{
    std::vector <cv::Point> points;
    for (int i = start; i <= end; ++i)
    {
        points.push_back(cv::Point(d.part(i).x(), d.part(i).y()));
    }
    cv::polylines(out, points, isClosed, cv::Scalar(161, 209, 255), 1, 16);
}

static void render_face (cv::Mat &out, const dlib::full_object_detection& d)
{
    draw_polyline(out, d, 0, 16);           // Jaw line
    draw_polyline(out, d, 17, 21);          // Left eyebrow
    draw_polyline(out, d, 22, 26);          // Right eyebrow
    draw_polyline(out, d, 27, 30);          // Nose bridge
    draw_polyline(out, d, 30, 35, true);    // Lower nose
    draw_polyline(out, d, 36, 41, true);    // Left eye
    draw_polyline(out, d, 42, 47, true);    // Right Eye
    draw_polyline(out, d, 48, 59, true);    // Outer lip
    draw_polyline(out, d, 60, 67, true);    // Inner lip

}

static void rounded_rectangle(cv::Mat& out, cv::Rect rect, const cv::Scalar lineColor, const int thickness, const int lineType , const int cornerRadius)
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
    line(out, Point (p1.x+cornerRadius,p1.y), Point (p2.x-cornerRadius,p2.y), lineColor, thickness, lineType);
    line(out, Point (p2.x,p2.y+cornerRadius), Point (p3.x,p3.y-cornerRadius), lineColor, thickness, lineType);
    line(out, Point (p4.x+cornerRadius,p4.y), Point (p3.x-cornerRadius,p3.y), lineColor, thickness, lineType);
    line(out, Point (p1.x,p1.y+cornerRadius), Point (p4.x,p4.y-cornerRadius), lineColor, thickness, lineType);

    // draw arcs
    ellipse( out, p1+Point(cornerRadius, cornerRadius), Size( cornerRadius, cornerRadius ), 180.0, 0, 90, lineColor, thickness, lineType );
    ellipse( out, p2+Point(-cornerRadius, cornerRadius), Size( cornerRadius, cornerRadius ), 270.0, 0, 90, lineColor, thickness, lineType );
    ellipse( out, p3+Point(-cornerRadius, -cornerRadius), Size( cornerRadius, cornerRadius ), 0.0, 0, 90, lineColor, thickness, lineType );
    ellipse( out, p4+Point(cornerRadius, -cornerRadius), Size( cornerRadius, cornerRadius ), 90.0, 0, 90, lineColor, thickness, lineType );
}

static void put_text(cv::Mat &out, 
                 const std::string str, const int x, const int y,
                 const int fontFace, const double fontScale, const cv::Scalar color,
                 const int thickness, const int lineType)
{
    cv::Point pos(x,y);
    cv::putText(out, str, pos, fontFace, fontScale, color, thickness, lineType);
}

void FaceDetector::demo(cv::Mat &out, const int frame, std::vector<dlib::rectangle> faces){

    static std::vector<dlib::rectangle> init_faces = faces;    
    static int startFrom = 0;
    static int skipFrames;    
    static int expectedNumFaces;
    const double fps = 30; // 1 second
    
    if(startFrom == 0 ){// from the beginning
        // Dima + Slava
        startFrom = frame;
        skipFrames = 3.5*fps; // delta
        expectedNumFaces = 2;
        LOG_INFO("Looking for Dima, Slava fn=" << startFrom);
    }else if(frame == 43*fps){// Stas appears
        // Stas
        startFrom = frame;
        skipFrames = 0*fps; // delta
        expectedNumFaces = 1;
        LOG_INFO("Looking for Stas fn=" << startFrom);
    }

    const int ajustedFN = frame - startFrom - skipFrames;
    if(ajustedFN < 0){
        return;
    }
    if(ajustedFN % 10 == 1){
        LOG_INFO("Faces found " << faces.size() << " fn=" << ajustedFN);
    }
    // start
    static int rectOn = 0;
    static int rectOff = -1;
    const int blinkRatio = fps / 4; // 1/6 second
    if(ajustedFN < 2*fps){
        // blink rects
        if(rectOn >= 0 && rectOn < blinkRatio){
            effect(out, faces, 0);
            rectOn++;
        }
        if(rectOff >= 0 && rectOff < blinkRatio){
            rectOff++;
        }
        if(rectOn == blinkRatio){
            rectOn = -1;
            rectOff = 0;
        }
        if(rectOff == blinkRatio){
            rectOn = 0;
            rectOff = -1;
        }
    } else if(ajustedFN < 4*fps){
        rectOn = 0;
        rectOff = -1;
        // draw rects
        effect(out, faces, 0);
    } else if(ajustedFN < 6*fps){
        // draw landmarks
        effect(out, faces, 1);
    } else if(ajustedFN < 12*fps){
        // put text
        effect(out, faces, 2);
    }
}

void FaceDetector::effect(cv::Mat&out, const std::vector<dlib::rectangle> &faces, const int effect){
    std::string d_s[] = {"Slava", "Dima"};
    std::string stas[] = {"Stas", "Unknown"};//sorry
    std::string persons[2] = faces.size() == 2 ? d_s : stas;

    static std::string suffix(" a Pixty founder");
	for (unsigned long i = 0; i < faces.size(); ++i)
	{
        dlib:Rectangle f = faces[i];
		cv::Rect cv_f(f.left(), f.top(), f.width(), f.height());
        cv_image<bgr_pixel> cout(out);
        
        std::string txt = persons[i] + suffix;
        // i {0, 1} -> Person1___Person2
        int x;
        if (faces.size() == 2){
            x = out.size().width/2.0 - txt.length() * 16.0 * (1 - i) + (i * 32.0);
        }else{
            x = out.size().width/2.0 - txt.length() * 8.0 ;            
        }
        int y = y = out.size().height - 32;            
        switch(effect){
            case 0:
                    rounded_rectangle(out, cv_f, cv::Scalar(58, 242, 252), 1, 16, 5);
                    break;
            case 1:
		            // Landmark detection on full sized image
		            // Custom Face Render
		            render_face(out, _pose_model(cout, f));
                    break;
		    case 2:                    
                    put_text(out, txt, x, y);
                    break;
            default:
                    //do nothing
                    break;
        }
	}
}

}

