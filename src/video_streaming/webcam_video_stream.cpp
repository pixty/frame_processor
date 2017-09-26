/*
 * webcam_video_stream.cpp
 *
 *  Created on: Jan 26, 2017
 *      Author: dmitry
 */

#include "webcam_video_stream.hpp"

namespace fproc {

WebcamVideoStream::WebcamVideoStream(const CameraParameters &cameraParameters):
        VideoStream(
                std::unique_ptr<cv::VideoCapture>(new cv::VideoCapture())), _fn(
                ts_now()),
        _cameraParameters(cameraParameters)
{
    _cap->open(_cameraParameters.camId);
    LOG_INFO("Creating WebcamVideoStream from cameraId=" << _cameraParameters.camId << ", first frame id=" << _fn);
    LOG_INFO("Initial camera parameters:");
    printActualCameraParameters();
    LOG_INFO("Setting camera parameters:");
    setCameraParameters();
    LOG_INFO("Updated camera parameters:");
    printActualCameraParameters();
    PFrame frame = captureFrame();
    LOG_INFO("Actual frame size: height="
             << frame->get_mat().size().height
             << ", width=" << frame->get_mat().size().width);
}


PFrame WebcamVideoStream::captureFrame() {
	Frame* frame = new Frame(_fn++, ts_now());
	*_cap >> frame->get_mat();
    return PFrame(frame);
}

void WebcamVideoStream::setCameraParameters(){
    if(this->_cameraParameters.width > 0){
        LOG_INFO("Setting frame width=" << this->_cameraParameters.width);
        bool success = _cap->set(cv::CAP_PROP_FRAME_WIDTH, this->_cameraParameters.width);
        LOG_INFO("" << ((success) ? "Success" : "Failed"));
    }
    if(this->_cameraParameters.height > 0){
        LOG_INFO("Setting frame height=" << this->_cameraParameters.height);
        bool success = _cap->set(cv::CAP_PROP_FRAME_HEIGHT,this->_cameraParameters.height);
        LOG_INFO("" << ((success) ? "Success" : "Failed"));
    }
    if(this->_cameraParameters.fps > 0){
        LOG_INFO("Setting FPS=" << this->_cameraParameters.fps);
        bool success = _cap->set(cv::CAP_PROP_FPS,this->_cameraParameters.fps);
        LOG_INFO("" << ((success) ? "Success" : "Failed"));
    }
    if(this->_cameraParameters.brightness >= 0){
        LOG_INFO("Setting brightness=" << this->_cameraParameters.brightness);
        bool success = _cap->set(cv::CAP_PROP_BRIGHTNESS,this->_cameraParameters.brightness);
        LOG_INFO("" << ((success) ? "Success" : "Failed"));
    }
    if(this->_cameraParameters.contrast >= 0){
        LOG_INFO("Setting contrast=" << this->_cameraParameters.contrast);
        bool success = _cap->set(cv::CAP_PROP_CONTRAST,this->_cameraParameters.contrast);
        LOG_INFO("" << ((success) ? "Success" : "Failed"));
    }
    if(this->_cameraParameters.saturation >= 0){
        LOG_INFO("Setting saturation=" << this->_cameraParameters.saturation);
        bool success = _cap->set(cv::CAP_PROP_SATURATION,this->_cameraParameters.saturation);
        LOG_INFO("" << ((success) ? "Success" : "Failed"));
    }
    if(this->_cameraParameters.hue >= 0){
        LOG_INFO("Setting HUE=" << this->_cameraParameters.hue);
        bool success = _cap->set(cv::CAP_PROP_HUE,this->_cameraParameters.hue);
        LOG_INFO("" << ((success) ? "Success" : "Failed"));
    }
    if(this->_cameraParameters.gain >= 0){
        LOG_INFO("Setting gain=" << this->_cameraParameters.gain);
        bool success = _cap->set(cv::CAP_PROP_GAIN,this->_cameraParameters.gain);
        LOG_INFO("" << ((success) ? "Success" : "Failed"));
    }
    if(this->_cameraParameters.auto_exposure > 0){
        LOG_INFO("Setting auto exposure=" << this->_cameraParameters.auto_exposure);
        bool success = _cap->set(cv::CAP_PROP_AUTO_EXPOSURE, this->_cameraParameters.auto_exposure);
        LOG_INFO("" << ((success) ? "Success" : "Failed"));
    }
    if(this->_cameraParameters.auto_exposure < 0 and this->_cameraParameters.exposure >= 0){
        LOG_INFO("Setting exposure=" << this->_cameraParameters.exposure);
        bool success = _cap->set(cv::CAP_PROP_EXPOSURE,this->_cameraParameters.exposure);
        LOG_INFO("" << ((success) ? "Success" : "Failed"));
    }
    if(this->_cameraParameters.sharpness >= 0){
        LOG_INFO("Setting sharpness=" << this->_cameraParameters.sharpness);
        bool success = _cap->set(cv::CAP_PROP_SHARPNESS,this->_cameraParameters.sharpness);
        LOG_INFO("" << ((success) ? "Success" : "Failed"));
    }
    if(this->_cameraParameters.gamma >= 0){
        LOG_INFO("Setting gamma=" << this->_cameraParameters.gamma);
        bool success = _cap->set(cv::CAP_PROP_GAMMA,this->_cameraParameters.gamma);
        LOG_INFO("" << ((success) ? "Success" : "Failed"));
    }
    if(this->_cameraParameters.temperature >= 0){
        LOG_INFO("Setting temperature=" << this->_cameraParameters.temperature);
        bool success = _cap->set(cv::CAP_PROP_TEMPERATURE,this->_cameraParameters.temperature);
        LOG_INFO("" << ((success) ? "Success" : "Failed"));
    }
    if(this->_cameraParameters.backlight >= 0){
        LOG_INFO("Setting backlight=" << this->_cameraParameters.backlight);
        bool success = _cap->set(cv::CAP_PROP_BACKLIGHT,this->_cameraParameters.backlight);
        LOG_INFO("" << ((success) ? "Success" : "Failed"));
    }
    {
        LOG_INFO("Setting autofocus=" << this->_cameraParameters.autofocus);
        bool success = _cap->set(cv::CAP_PROP_AUTOFOCUS,this->_cameraParameters.autofocus);
        LOG_INFO("" << ((success) ? "Success" : "Failed"));
    }
    {
        LOG_INFO("Setting convert to rgb");
        bool success = _cap->set(cv::CAP_PROP_CONVERT_RGB,1);
        LOG_INFO("" << ((success) ? "Success" : "Failed"));
    }
    if(this->_cameraParameters.fourcc > 0){
        LOG_INFO("Setting fourcc=" << this->_cameraParameters.fourcc);
        bool success = _cap->set(cv::CAP_PROP_FOURCC,this->_cameraParameters.fourcc);
        LOG_INFO("" << ((success) ? "Success" : "Failed"));
    }
}

void WebcamVideoStream::printActualCameraParameters(){
    LOG_INFO("Frame width=" << _cap->get(cv::CAP_PROP_FRAME_WIDTH));
    LOG_INFO("Frame height=" << _cap->get(cv::CAP_PROP_FRAME_HEIGHT));
    LOG_INFO("FPS=" << _cap->get(cv::CAP_PROP_FPS));
    LOG_INFO("Brightness=" << _cap->get(cv::CAP_PROP_BRIGHTNESS));
    LOG_INFO("Contrast=" << _cap->get(cv::CAP_PROP_CONTRAST));
    LOG_INFO("Saturation=" << _cap->get(cv::CAP_PROP_SATURATION));
    LOG_INFO("Gain=" << _cap->get(cv::CAP_PROP_GAIN));
    LOG_INFO("Exposure=" << _cap->get(cv::CAP_PROP_EXPOSURE));
    LOG_INFO("Auto Exposure=" << _cap->get(cv::CAP_PROP_AUTO_EXPOSURE));
    LOG_INFO("Autofocus=" << _cap->get(cv::CAP_PROP_AUTOFOCUS));
    LOG_INFO("Covert to RGB=" << _cap->get(cv::CAP_PROP_CONVERT_RGB));
    int rfcc = static_cast<int>(_cap->get(cv::CAP_PROP_FOURCC));
    // Transform from int to char via Bitwise operators
    char fcc[] = {(char)(rfcc & 0XFF),
                  (char)((rfcc & 0XFF00) >> 8),
                  (char)((rfcc & 0XFF0000) >> 16),
                  (char)((rfcc & 0XFF000000) >> 24),
                  0};
    LOG_INFO("fourcc=" << fcc);
    LOG_INFO("format=" << _cap->get(cv::CAP_PROP_FORMAT));
    // TODO: These are not supported properly by openc+ubuntu
    LOG_INFO("HUE=" << _cap->get(cv::CAP_PROP_HUE));
    LOG_INFO("Sharpness=" << _cap->get(cv::CAP_PROP_SHARPNESS));
    LOG_INFO("Gamma=" << _cap->get(cv::CAP_PROP_GAMMA));
    LOG_INFO("Temperature=" << _cap->get(cv::CAP_PROP_TEMPERATURE));
    LOG_INFO("Backlight=" << _cap->get(cv::CAP_PROP_BACKLIGHT));
}

}
