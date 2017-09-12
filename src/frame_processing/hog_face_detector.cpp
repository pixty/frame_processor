#include "../frame_processing/object_detectors.hpp"
#include <dlib/image_transforms.h>

namespace fproc {

HogFaceDetector::HogFaceDetector() :
        _detector(dlib::get_frontal_face_detector()) {
}

const PFrameRegList & HogFaceDetector::detectRegions(PFrame pFrame) {
    dlib::array2d<unsigned char> img_gray;
    dlib::assign_image(img_gray, pFrame->get_bgr_image());
    std::vector<Rectangle> objs = _detector(img_gray);

    _objects.clear();
    for (Rectangle& r : objs) {
        PFrameRegion pfr(new FrameRegion(pFrame, r));
        _objects.push_back(pfr);
    }

    return _objects;
}

}

