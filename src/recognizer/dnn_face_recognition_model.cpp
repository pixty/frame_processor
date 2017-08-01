#include "./dnn_face_recognition_model.hpp"
#include <dlib/dnn.h>
#include <dlib/image_io.h>
#include <dlib/image_processing/frontal_face_detector.h>

namespace fproc {

using namespace dlib;
using namespace std;

DnnFaceRecognitionNet::DnnFaceRecognitionNet(const string &dnn_net_model_fn,
		const string &face_landmarks_model_fn) {
	deserialize(dnn_net_model_fn) >> _net;
	deserialize(face_landmarks_model_fn) >> _shape_predictor;
}

const V128Ds DnnFaceRecognitionNet::face_descriptors(
		const matrix<rgb_pixel> img, const BBoxes bounding_boxes) {
	std::vector<matrix<rgb_pixel>> faces;
	for (auto face : bounding_boxes) {
		auto shape = _shape_predictor(img, face);
		matrix<rgb_pixel> face_chip;
		extract_image_chip(img, get_face_chip_details(shape, 150, 0.25),
				face_chip);
		faces.push_back(move(face_chip));
	}
	return _net(faces);
}

void DnnFaceRecognitionNet::set_vector(PFrameRegion fr) {
	Frame::DlibRgbImg& img = fr->getFrame()->get_rgb_image();
	auto shape = _shape_predictor(img, fr->getRectangle());
	matrix<rgb_pixel> face_chip;
	extract_image_chip(img, get_face_chip_details(shape, 150, 0.25), face_chip);
	fr->set_vector(_net(face_chip));
}

}
