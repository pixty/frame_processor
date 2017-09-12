#include "./dnn_face_recognition_model.hpp"

namespace fproc {

using namespace dlib;
using namespace std;

DnnFaceRecognitionNet::DnnFaceRecognitionNet(const string &dnn_net_model_fn,
		const string &face_landmarks_model_fn) {
	deserialize(dnn_net_model_fn) >> _net;
	deserialize(face_landmarks_model_fn) >> _shape_predictor;
}

void DnnFaceRecognitionNet::set_vector(PFrame f, PFrameRegion fr) {
	Frame::DlibRgbImg& img = f->get_rgb_image();
	auto shape = _shape_predictor(img, fr->getRectangle());
	matrix<rgb_pixel> face_chip;
	extract_image_chip(img, get_face_chip_details(shape, 150, 0.25), face_chip);
	fr->set_vector(_net(face_chip));
}

}
