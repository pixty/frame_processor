/*
 * image_processing.cpp
 *
 *  Created on: Sep 18, 2017
 *      Author: dmitry
 */

#include "image_processing.hpp"
#include "../logger.hpp"

namespace fproc {

bool operator >= (const Size& sz1, const Size& sz2) {
	return sz1.height >= sz2.height && sz1.width >= sz2.width;
}

bool operator >(ImageSize::Code c1, ImageSize::Code c2) {
	return ImageSize::CODE_TO_IDX_MAP.at(c1) > ImageSize::CODE_TO_IDX_MAP.at(c2);
}

const Size ImageSize::ORIGINAL_SIZE = Size(0, 0);
const Size ImageSize::IMG_SZ_800x600_SIZE = Size(800, 600);
const Size ImageSize::IMG_SZ_640x480_SIZE = Size(640, 480);
const Size ImageSize::IMG_SZ_320x240_SIZE = Size(320, 240);
const Size ImageSize::IMG_SZ_160x120_SIZE = Size(160, 120);

const std::map<ImageSize::Code, int> ImageSize::CODE_TO_IDX_MAP = {{ImageSize::ORIGINAL, 4},
		{ImageSize::IMG_SZ_800x600, 3}, {ImageSize::IMG_SZ_640x480, 2}, {ImageSize::IMG_SZ_320x240, 1}, {ImageSize::IMG_SZ_160x120, 0}};
const ImageSize::Code ImageSize::CODES[] = {ImageSize::IMG_SZ_160x120, ImageSize::IMG_SZ_320x240,
		ImageSize::IMG_SZ_640x480, ImageSize::IMG_SZ_800x600, ImageSize::ORIGINAL};
const Size ImageSize::CODE_SIZES[] = {ImageSize::IMG_SZ_160x120_SIZE, ImageSize::IMG_SZ_320x240_SIZE,
		ImageSize::IMG_SZ_640x480_SIZE, ImageSize::IMG_SZ_800x600_SIZE, ImageSize::ORIGINAL_SIZE};
const int ImageSize::CODES_COUNT = sizeof(ImageSize::CODES)/sizeof(*ImageSize::CODES);

Size ImageSize::adjustSize(Code code, const Size& orgSz) {
	Size esz = getSize(code);
	if (esz.width <= 0 || orgSz.width <= 0 || orgSz.height <= 0
			|| orgSz.width <= 0) {
		return orgSz;
	}

	if (esz.width > orgSz.width && esz.height > orgSz.height) {
		return orgSz;
	}

	double ddw = double(esz.width) / orgSz.width;
	double ddh = double(esz.height) / orgSz.height;
	if (ddw < ddh) {
		esz.height = std::max(1, int(floor(ddw * orgSz.height)));
	} else {
		esz.width = std::max(1, int(floor(ddh * orgSz.width)));
	}

	return esz;
}

// Returns a code which covers the size, if no one returns original
ImageSize::Code ImageSize::getMajor(const Size& sz) {
	for (int i = 0; i < CODES_COUNT; i++) {
		if (CODE_SIZES[i] >= sz) {
			return CODES[i];
		}
	}
	return ORIGINAL;
}

// Returns a code which is covered by the size, or original if no one
ImageSize::Code ImageSize::getMinorCode(const Size& sz) {
	for (int i = CODES_COUNT-1; i >= 0; i--) {
		if (sz >= CODE_SIZES[i]) {
			return CODES[i];
		}
	}
	return ORIGINAL;
}

static bool compress_mat(cv::Mat &mat, std::vector<uchar>& res_buf, CompType cmp_tp, int quality) {
	std::vector<int> compression_params;
	std::string ext;
	if (cmp_tp == CompType::JPEG) {
		compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
		compression_params.push_back(quality);
		res_buf.reserve(20000);
		ext = ".jpg";
	} else if (cmp_tp == CompType::PNG) {
		compression_params.push_back(cv::IMWRITE_PNG_COMPRESSION);
		compression_params.push_back(quality);
		res_buf.reserve(1000000);
		ext = ".png";
	} else {
		LOG_ERROR("Unexpected compression type " << cmp_tp << ", do nothing.");
		return false;
	}

	try {
		cv::imencode(ext, mat, res_buf, compression_params);
	} catch (cv::Exception& ex) {
		LOG_ERROR("Exception converting image to PNG format: %s\n" << ex.what());
		return false;
	}
	return true;
}

static Size compress_and_resize_mat(cv::Mat &mat, ImageSize::Code code, const Size& sz, std::vector<uchar>& res_buf, CompType cmp_tp, int quality) {
	// adjusted size
	Size asz = ImageSize::adjustSize(code, sz);
	if (sz == asz) {
		return compress_mat(mat, res_buf, cmp_tp, quality) ? sz : ImageSize::ORIGINAL_SIZE;
	}
	cv::Mat dst;
	cv::resize(mat, dst, asz);
	return compress_mat(dst, res_buf, cmp_tp, quality) ? asz : ImageSize::ORIGINAL_SIZE;
}

Size compress_frame(PFrame pf, ImageSize::Code code, std::vector<uchar>& res_buf, CompType cmp_tp, int quality) {
	return compress_and_resize_mat(pf->get_mat(), code, pf->size(), res_buf, cmp_tp, quality);
}

Size compress_frame_region(PFrame pf, const Rectangle& rect, ImageSize::Code code, int border, std::vector<uchar>& res_buf, CompType cmp_tp, int quality) {
	Rectangle r = addBorder(rect, pf->size(), border);
	Size rsz(r.width(), r.height());
	if (rsz == pf->size()) {
		return compress_frame(pf, code, res_buf, cmp_tp, quality);
	}

	CvRect cvr = toCvRect(r);
	cv::Mat cropped_mat = pf->get_mat()(cvr);
	return compress_and_resize_mat(cropped_mat, code, rsz, res_buf, cmp_tp, quality);
}

}

