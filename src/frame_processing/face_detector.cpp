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

static double FPS = 30;

static void render_face(cv::Mat &out, const dlib::full_object_detection& d);
static void rounded_rectangle(cv::Mat& out, cv::Rect rect,
		const cv::Scalar lineColor = cv::Scalar(58, 242, 252),
		const int thickness = 1, const int lineType = 16,
		const int cornerRadius = 8);
static void put_text(cv::Mat &out, const std::string str, const int x,
		const int y, const int fontFace = cv::FONT_HERSHEY_COMPLEX_SMALL,
		const double fontScale = 0.8,
		const cv::Scalar color = cv::Scalar(255, 28, 20),
		const int thickness = 2, const int lineType = 8);
static cv::Rect outerRect(const dlib::full_object_detection& d);
static int triangle(const int ratio);
static void resetTriangle();

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

static void draw_polyline(cv::Mat &out, const dlib::full_object_detection& d,
		const int start, const int end, bool isClosed = false) {
	std::vector<cv::Point> points;
	for (int i = start; i <= end; ++i) {
		points.push_back(cv::Point(d.part(i).x(), d.part(i).y()));
	}
	cv::polylines(out, points, isClosed, cv::Scalar(161, 209, 255), 1, 16);
}

static void render_face(cv::Mat &out, const dlib::full_object_detection& d) {
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

static void rounded_rectangle(cv::Mat& out, cv::Rect rect,
		const cv::Scalar lineColor, const int thickness, const int lineType,
		const int cornerRadius) {
	using namespace cv;
	/* corners:
	 * p1 - p2
	 * |     |
	 * p4 - p3
	 */
	cv::Point topLeft(rect.tl());
	cv::Point bottomRight(rect.br());

	Point p1 = topLeft;
	Point p2 = Point(bottomRight.x, topLeft.y);
	Point p3 = bottomRight;
	Point p4 = Point(topLeft.x, bottomRight.y);

	// draw straight lines
	line(out, Point(p1.x + cornerRadius, p1.y),
			Point(p2.x - cornerRadius, p2.y), lineColor, thickness, lineType);
	line(out, Point(p2.x, p2.y + cornerRadius),
			Point(p3.x, p3.y - cornerRadius), lineColor, thickness, lineType);
	line(out, Point(p4.x + cornerRadius, p4.y),
			Point(p3.x - cornerRadius, p3.y), lineColor, thickness, lineType);
	line(out, Point(p1.x, p1.y + cornerRadius),
			Point(p4.x, p4.y - cornerRadius), lineColor, thickness, lineType);

	// draw arcs
	ellipse(out, p1 + Point(cornerRadius, cornerRadius),
			Size(cornerRadius, cornerRadius), 180.0, 0, 90, lineColor,
			thickness, lineType);
	ellipse(out, p2 + Point(-cornerRadius, cornerRadius),
			Size(cornerRadius, cornerRadius), 270.0, 0, 90, lineColor,
			thickness, lineType);
	ellipse(out, p3 + Point(-cornerRadius, -cornerRadius),
			Size(cornerRadius, cornerRadius), 0.0, 0, 90, lineColor, thickness,
			lineType);
	ellipse(out, p4 + Point(cornerRadius, -cornerRadius),
			Size(cornerRadius, cornerRadius), 90.0, 0, 90, lineColor, thickness,
			lineType);
}

static void put_text(cv::Mat &out, const std::string str, const int x,
		const int y, const int fontFace, const double fontScale,
		const cv::Scalar color, const int thickness, const int lineType) {
	cv::Point pos(x, y);
	cv::putText(out, str, pos, fontFace, fontScale, color, thickness, lineType);
}

void FaceDetector::demo(cv::Mat &out, const int frame,
		std::vector<dlib::rectangle> faces) {

	static std::vector<dlib::rectangle> init_faces = faces;
	static int startFrom = 0;
	static int skipFrames;

	if (startFrom == 0) {    // from the beginning
		if (faces.size() != 2)
			return;
		// Dima + Slava
		startFrom = frame;
		skipFrames = 0 * FPS; // delta
		expectedNumFaces = 2;
		resetTriangle();
		latest.clear();
		LOG_INFO("Looking for Dima, Slava fn=" << startFrom);
	} else if (frame == 43 * FPS) { // Stas appears
	// Stas
		startFrom = frame;
		skipFrames = 0 * FPS; // delta
		expectedNumFaces = 1;
		resetTriangle();
		latest.clear();
		LOG_INFO(
				"Looking for Stas fn=" << startFrom << " faces=" << faces.size());
	}

	const int adjustedFN = frame - startFrom - skipFrames;
	if (adjustedFN < 0) {
		return;
	}
	if (adjustedFN % 10 == 1) {
		LOG_INFO("Faces found " << faces.size() << " fn=" << adjustedFN);
	}
	// startse
	if (adjustedFN < 1.5 * FPS) {
		// blink rects
		effect(out, faces, 3);
	} else if (adjustedFN < 1.8 * FPS) {
		resetTriangle();
		// solid + points
		effect(out, faces, 1);
	} else if (adjustedFN < 3.5 * FPS) {
		// solid + ...
		effect(out, faces, 4);
	} else {
		// solid + name
		effect(out, faces, 2);
	}
}

void FaceDetector::effect(cv::Mat&out,
		const std::vector<dlib::rectangle> &faces, const int effect) {
	//std::string persons[] = { "Slava", "Dima", "Stas" };
	std::string persons[] = { "Slava", "Dmitry", "Stas" };
	//static std::string suffix(" a Pixty founder");
	static std::string suffix("");
	static int callCount = 0;
	cv::Scalar txtColor = cv::Scalar(252, 239, 201);
	cv::Scalar brdrColor = cv::Scalar(58, 242, 252);

	callCount++;
	if (expectedNumFaces == faces.size()) {
		latest = faces;
	}
	for (unsigned long i = 0; i < faces.size(); ++i) {
		dlib: Rectangle f = faces[i];
		cv::Rect cv_f(f.left(), f.top(), f.width(), f.height());
		cv_image<bgr_pixel> cout(out);

		std::string txt = persons[i] + suffix;
		full_object_detection shape = _pose_model(cout, f);
		cv::Rect outerR = outerRect(shape);
		// i {0, 1} -> Person1___Person2
		int x;
		if (faces.size() == 2) {
			if (outerR.x < out.size().width / 2) {
				x = outerR.x;
				txt = persons[0] + suffix;
			} else {
				x = outerR.x;
				txt = persons[1] + suffix;
			}
		} else {
			x = outerR.x - 32;
			txt = persons[2] + suffix;

			txtColor = cv::Scalar(0, 206, 3);
			brdrColor = cv::Scalar(0, 206, 3);
		}
		cv::Scalar color = (58, 242, 252);
		//int y = outerR.y - 16;
		int y = outerR.y + outerR.height + 40;
		int phase = 0;

		switch (effect) {
		case 0:
			rounded_rectangle(out, outerR);
			break;
		case 1:
			// solid + points
			rounded_rectangle(out, outerR, cv::Scalar(70, 70, 70), 3);
			rounded_rectangle(out, outerR, cv::Scalar(255, 255, 255), 1);
			// Landmark detection on full sized image
			// Custom Face Render
			render_face(out, shape);
			break;
		case 2:
			rounded_rectangle(out, outerR, brdrColor);
			put_text(out, txt, x, y, cv::FONT_HERSHEY_COMPLEX_SMALL, 1.3, txtColor);
			put_text(out, "Pixty founder", x, y + 40, cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, txtColor);
			break;
		case 3:
			// blinking
			phase = triangle(FPS / 2);
			rounded_rectangle(out, outerR,
					cv::Scalar(5 * phase, 16 * phase, 15 * phase));
					/*cv::Scalar(58, 242, 252, 17*phase));*/
			break;
		case 4:
			//phase = triangle(3*FPS) / 5 + 1;
			//phase = triangle(3*FPS) / (FPS/3) + 1;
			phase = (int)(callCount / (FPS/3)) % 3 + 1;
			rounded_rectangle(out, outerR, brdrColor);
			//put_text(out, "Looking " + std::string(phase, '.'), x, y);
			put_text(out, "Looking " + std::string(phase, '.'), x, y,
					cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, txtColor);
			break;
		default:
			//do nothing
			break;
		}
	}
}

static cv::Rect outerRect(const dlib::full_object_detection& d) {
	int l = 65536;
	int r = 0;
	int t = 65536;
	int b = 0;
	for (int i = 0; i < d.num_parts(); i++) {
		int x = d.part(i).x();
		int y = d.part(i).y();
		if (x < l)
			l = x;
		if (x > r)
			r = x;
		if (y < t)
			t = y;
		if (y > b)
			b = y;
	}
	return cv::Rect(l, t, r - l, b - t);
}

static int triangle(const int ratio) {
	static int state = 0;
	static bool v = true;
	if (ratio < 1) {
		// reset
		state = 0;
		v = true;
	} else if (state < ratio) {
		state++;
	} else {
		state = 1;
		v = !v;
	}
	return v ? state : ratio - state;
}

static void resetTriangle() {
	triangle(-1);
}

}

