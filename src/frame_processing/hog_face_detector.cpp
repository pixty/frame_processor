#include "../frame_processing/object_detectors.hpp"
#include <dlib/image_transforms.h>
#include<math.h>

namespace fproc {

HogFaceDetector::HogFaceDetector() :
        _detector(dlib::get_frontal_face_detector()) {
}

const PFrameRegList & HogFaceDetector::detectRegions(PFrame pFrame) {
    std::vector<Rectangle> objs;
    const Frame::DlibBgrImg& original = pFrame->get_bgr_image();
    const double x_ratio = ((double)original.nc())/((double)_params.width);
    const double y_ratio = ((double)original.nr())/((double)_params.height);
    if (_params.grayscale){
        dlib::array2d<unsigned char> img_gray_src;
        pFrame->get_bgr_image();
        dlib::assign_image(img_gray_src, original);
        if(_params.height > 0){
            dlib::array2d<unsigned char> img_gray_dst(_params.height, _params.width);
            dlib::resize_image(img_gray_src, img_gray_dst);
            objs = _detector(img_gray_dst);
        }else{
            objs = _detector(img_gray_src);
        }
    }else{
        if(_params.height > 0){
            dlib::array2d<dlib::bgr_pixel> img_bgr_dst(_params.height, _params.width);
            dlib::resize_image(original, img_bgr_dst);
            objs = _detector(img_bgr_dst);
        }else{
            objs = _detector(original);
        }
    }

    _objects.clear();
    for (Rectangle& r : objs) {

        if(_params.height > 0){
            r.set_left(floor(r.left()*x_ratio));
            r.set_top(floor(r.top()*y_ratio));
            r.set_right(ceil(r.right()*x_ratio));
            r.set_bottom(ceil(r.bottom()*y_ratio));
        }

        PFrameRegion pfr(new FrameRegion(pFrame, r));
        _objects.push_back(pfr);
    }

    return _objects;
}

}

