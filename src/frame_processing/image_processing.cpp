/*
 * image_processing.cpp
 *
 *  Created on: Sep 18, 2017
 *      Author: dmitry
 */

#include "image_processing.hpp"
#include "../logger.hpp"

namespace fproc {

bool operator >=(const Size& sz1, const Size& sz2) {
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

const std::map<ImageSize::Code, int> ImageSize::CODE_TO_IDX_MAP = { {
		ImageSize::ORIGINAL, 4 }, { ImageSize::IMG_SZ_800x600, 3 }, {
		ImageSize::IMG_SZ_640x480, 2 }, { ImageSize::IMG_SZ_320x240, 1 }, {
		ImageSize::IMG_SZ_160x120, 0 } };
const ImageSize::Code ImageSize::CODES[] = { ImageSize::IMG_SZ_160x120,
		ImageSize::IMG_SZ_320x240, ImageSize::IMG_SZ_640x480,
		ImageSize::IMG_SZ_800x600, ImageSize::ORIGINAL };
const Size ImageSize::CODE_SIZES[] = { ImageSize::IMG_SZ_160x120_SIZE,
		ImageSize::IMG_SZ_320x240_SIZE, ImageSize::IMG_SZ_640x480_SIZE,
		ImageSize::IMG_SZ_800x600_SIZE, ImageSize::ORIGINAL_SIZE };
const int ImageSize::CODES_COUNT = sizeof(ImageSize::CODES)
		/ sizeof(*ImageSize::CODES);

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
	for (int i = CODES_COUNT - 1; i >= 0; i--) {
		if (sz >= CODE_SIZES[i]) {
			return CODES[i];
		}
	}
	return ORIGINAL;
}

struct mat_compressor {
	mat_compressor(ImageSize::Code code, CompType cmp_tp, int quality) : code_(code),
			cmp_tp_(cmp_tp), quality_(quality) {
	}
	;
	virtual ~mat_compressor() {
	}

	Size compress_frame(PFrame pf) {
		return compress_and_resize_mat(pf->get_mat(), pf->size());
	}

	Size compress_frame_region(PFrame pf, const Rectangle& rect, int border);
	Size compress_and_resize_mat(cv::Mat &mat, const Size& sz);
	bool compress_mat(cv::Mat &mat);
	virtual void compress_mat_and_store(cv::Mat &mat,
			std::vector<int>& compression_params) = 0;

	ImageSize::Code code_;
	CompType cmp_tp_;
	int quality_;
};

struct inmem_mat_compressor: public mat_compressor {
	inmem_mat_compressor(ImageSize::Code code, CompType cmp_tp, int quality,
			std::vector<uchar>& res_buf) :
			mat_compressor(code, cmp_tp, quality), res_buf_(res_buf) {
		if (cmp_tp_ == CompType::JPEG) {
			res_buf_.reserve(20000);
			ext_ = ".jpg";
		} else if (cmp_tp_ == CompType::PNG) {
			res_buf_.reserve(1000000);
			ext_ = ".png";
		}
	}

protected:
	void compress_mat_and_store(cv::Mat &mat,
			std::vector<int>& compression_params) {
		cv::imencode(ext_, mat, res_buf_, compression_params);
	}
private:
	std::string ext_;
	std::vector<uchar>& res_buf_;
};

struct tofile_mat_compressor: public mat_compressor {
	tofile_mat_compressor(ImageSize::Code code, CompType cmp_tp, int quality, const std::string& filename): mat_compressor(code, cmp_tp, quality), filename_(filename) {}
protected:
	void compress_mat_and_store(cv::Mat &mat,
				std::vector<int>& compression_params) {
			cv::imwrite(filename_, mat, compression_params);
		}
private:
	const std::string filename_;
};


Size mat_compressor::compress_frame_region(PFrame pf, const Rectangle& rect, int border) {
	Rectangle r = addBorder(rect, pf->size(), border);
	Size rsz(r.width(), r.height());
	if (rsz == pf->size()) {
		return compress_frame(pf);
	}

	CvRect cvr = toCvRect(r);
	cv::Mat cropped_mat = pf->get_mat()(cvr);
	return compress_and_resize_mat(cropped_mat, rsz);
}

bool mat_compressor::compress_mat(cv::Mat &mat) {
	std::vector<int> compression_params;
	std::string ext;
	if (cmp_tp_ == CompType::JPEG) {
		compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
		compression_params.push_back(quality_);
	} else if (cmp_tp_ == CompType::PNG) {
		compression_params.push_back(cv::IMWRITE_PNG_COMPRESSION);
		compression_params.push_back(quality_);
	} else {
		LOG_ERROR("Unexpected compression type " << cmp_tp_ << ", do nothing.");
		return false;
	}

	try {
		compress_mat_and_store(mat, compression_params);
	} catch (cv::Exception& ex) {
		LOG_ERROR(
				"Exception converting image to PNG format: %s\n" << ex.what());
		return false;
	}
	return true;
}

Size mat_compressor::compress_and_resize_mat(cv::Mat &mat, const Size& sz) {
	// adjusted size
	Size asz = ImageSize::adjustSize(code_, sz);
	if (sz == asz) {
		return compress_mat(mat) ?
				sz : ImageSize::ORIGINAL_SIZE;
	}
	cv::Mat dst;
	cv::resize(mat, dst, asz);
	return compress_mat(dst) ?
			asz : ImageSize::ORIGINAL_SIZE;
}

Size compress_frame(PFrame pf, ImageSize::Code code,
		std::vector<uchar>& res_buf, CompType cmp_tp, int quality) {
	inmem_mat_compressor mc(code, cmp_tp, quality, res_buf);
	return mc.compress_frame(pf);
}

Size compress_frame_region(PFrame pf, const Rectangle& rect,
		ImageSize::Code code, int border, std::vector<uchar>& res_buf,
		CompType cmp_tp, int quality) {
	inmem_mat_compressor mc(code, cmp_tp, quality, res_buf);
	return mc.compress_frame_region(pf, rect, border);
}

void compress_frame_to_file(PFrame pf, ImageSize::Code code,
		std::string& filename, CompType cmp_tp, int quality) {
	tofile_mat_compressor mc(code, cmp_tp, quality, filename);
	mc.compress_frame(pf);
}

void compress_frame_region_to_file(PFrame pf, const Rectangle& rect,
		ImageSize::Code code, int border, std::string& filename,
		CompType cmp_tp, int quality) {
	LOG_INFO("Saving frame region " << rect << " to file " << filename);
	tofile_mat_compressor mc(code, cmp_tp, quality, filename);
	mc.compress_frame_region(pf, rect, border);
}

// OpenCV port of 'LAPM' algorithm (Nayar89)
static double modifiedLaplacian(const cv::Mat& src) {
    cv::Mat M = (cv::Mat_<double>(3, 1) << -1, 2, -1);
    cv::Mat G = cv::getGaussianKernel(3, -1, CV_64F);

    cv::Mat Lx;
    cv::sepFilter2D(src, Lx, CV_64F, M, G);

    cv::Mat Ly;
    cv::sepFilter2D(src, Ly, CV_64F, G, M);

    cv::Mat FM = cv::abs(Lx) + cv::abs(Ly);

    double focusMeasure = cv::mean(FM).val[0];
    return focusMeasure;
}

// This one shows best results in our tests, so will use it
// OpenCV port of 'LAPV' algorithm (Pech2000)
static double varianceOfLaplacian(const cv::Mat& src) {
    cv::Mat lap;
    cv::Laplacian(src, lap, CV_64F);

    cv::Scalar mu, sigma;
    cv::meanStdDev(lap, mu, sigma);

    double focusMeasure = sigma.val[0]*sigma.val[0];
    return focusMeasure;
}

// OpenCV port of 'TENG' algorithm (Krotkov86)
static double tenengrad(const cv::Mat& src, int ksize) {
    cv::Mat Gx, Gy;
    cv::Sobel(src, Gx, CV_64F, 1, 0, ksize);
    cv::Sobel(src, Gy, CV_64F, 0, 1, ksize);

    cv::Mat FM = Gx.mul(Gx) + Gy.mul(Gy);

    double focusMeasure = cv::mean(FM).val[0];
    return focusMeasure;
}

// OpenCV port of 'GLVN' algorithm (Santos97)
static double normalizedGraylevelVariance(const cv::Mat& src) {
    cv::Scalar mu, sigma;
    cv::meanStdDev(src, mu, sigma);

    double focusMeasure = (sigma.val[0]*sigma.val[0]) / mu.val[0];
    return focusMeasure;
}

double sharpness(PFrame pf, const Rectangle& rect) {
	CvRect cvr = toCvRect(rect);
	cv::Mat gray_img = pf->get_gray_mat()(cvr);
	return varianceOfLaplacian(gray_img);
}

}

