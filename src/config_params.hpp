/*
 * config_params.hpp
 *
 *  Created on: Sep 19, 2017
 *      Author: dmitry
 */

#ifndef SRC_CONFIG_PARAMS_HPP_
#define SRC_CONFIG_PARAMS_HPP_

namespace fproc {

struct CameraParameters {
	int width;
	int height;
	int fps;
	double brightness; // 0 - 1
	double contrast;
	double saturation;
	double gain;
	double sharpness;
	double temperature;
	double backlight;
	double exposure;
	double auto_exposure;
	bool autofocus;
	double gamma;
	double hue;
	int fourcc;
	CameraParameters() :
			width(-1), height(-1), fps(-1), contrast(-1), brightness(-1), saturation(
					-1), gain(-1), sharpness(-1), temperature(-1), backlight(
					-1), exposure(-1), auto_exposure(-1), autofocus(true), gamma(
					-1), hue(-1), fourcc(-1) {
	}
};

struct HogParameters {
	int width;
	int height;
	bool grayscale;
	HogParameters() :
			width(-1), height(-1), grayscale(true) {
	}
};

struct FpcpClientParameters {
	// go to localhost
	std::string address = "localhost:50051";
	std::string access_key;
	std::string secret_key;
	// 640x480, check frame_processing/image_processing.hpp
	char dflt_size_code = 'm';
	bool jpeg = true;
	int cut_face_border = 10;
	FpcpClientParameters() {
	}
	FpcpClientParameters(const FpcpClientParameters& pcp) :
			address(pcp.address), access_key(pcp.access_key), secret_key(
					pcp.secret_key), dflt_size_code(pcp.dflt_size_code), jpeg(
					pcp.jpeg), cut_face_border(pcp.cut_face_border) {
	}
};

}

#endif /* SRC_CONFIG_PARAMS_HPP_ */
