/*
 * image_window.cpp
 *
 *  Created on: Jan 26, 2017
 *      Author: dmitry
 */

#include "image_window.hpp"

namespace fproc {

	void ImageWindow::show(PFrame frame) {
		_win.set_image(frame->get_cv_image());
	}

}


