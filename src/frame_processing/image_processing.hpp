/*
 * image_processing.hpp
 *
 *  Created on: Sep 18, 2017
 *      Author: dmitry
 */

#ifndef SRC_FRAME_PROCESSING_IMAGE_PROCESSING_HPP_
#define SRC_FRAME_PROCESSING_IMAGE_PROCESSING_HPP_

#include "../model.hpp"
#include <algorithm>
#include <math.h>

namespace fproc {

struct ImageSize {
	typedef enum {
		ORIGINAL = 'o',
		IMG_SZ_800x600 = 'l',
		IMG_SZ_640x480 = 'm',
		IMG_SZ_320x240 = 's',
		IMG_SZ_160x120 = 't'
	} Code;

	static const Size ORIGINAL_SIZE;
	static const Size IMG_SZ_800x600_SIZE;
	static const Size IMG_SZ_640x480_SIZE;
	static const Size IMG_SZ_320x240_SIZE;
	static const Size IMG_SZ_160x120_SIZE;

	static const std::map<Code, int> CODE_TO_IDX_MAP;
	static const Code CODES[];
	static const Size CODE_SIZES[];
	static const int CODES_COUNT;

	static bool isValidCode(char c) { return CODE_TO_IDX_MAP.find(Code(c)) != CODE_TO_IDX_MAP.end(); }
	static const Size& getSize(Code code) { return CODE_SIZES[CODE_TO_IDX_MAP.at(code)];}
	// gets the code and returns smaller one, or original if no smaller
	static Code smaller(Code code) {
		int idx = CODE_TO_IDX_MAP.at(code);
		return idx > 0 ? CODES[idx - 1] : ORIGINAL;
	}
	static Size adjustSize(Code code, const Size& orgSz);
	// Returns a code which covers the size, if no one returns original
	static Code getMajor(const Size& sz);
	// Returns a code which is covered by the size, or original if no one
	static Code getMinorCode(const Size& sz);
};

bool operator >(ImageSize::Code c1, ImageSize::Code c2);

enum CompType {
	JPEG,
	PNG
};

// Compresses frame using the code size, to the res_buf and compression type with the quality param.
// quality [1..100] for JPEG and [0..10] for PNG
Size compress_frame(PFrame pf, ImageSize::Code code, std::vector<uchar>& res_buf, CompType cmp_tp, int quality);
Size compress_frame_region(PFrame pf, const Rectangle& rect, ImageSize::Code code, int border, std::vector<uchar>& res_buf, CompType cmp_tp, int quality);

void compress_frame_to_file(PFrame pf, ImageSize::Code code, std::string& filename, CompType cmp_tp, int quality);
void compress_frame_region_to_file(PFrame pf, const Rectangle& rect, ImageSize::Code code, int border, std::string& filename, CompType cmp_tp, int quality);

// Bigger is better
double sharpness(PFrame pf, const Rectangle& rect);

}

#endif /* SRC_FRAME_PROCESSING_IMAGE_PROCESSING_HPP_ */
