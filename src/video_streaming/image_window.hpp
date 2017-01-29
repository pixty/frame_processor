/*
 * image_window.hpp
 *
 *  Created on: Jan 26, 2017
 *      Author: dmitry
 */

#ifndef SRC_VIDEO_STREAMING_IMAGE_WINDOW_HPP_
#define SRC_VIDEO_STREAMING_IMAGE_WINDOW_HPP_

#include <dlib/gui_widgets.h>
#include "../model.hpp"

namespace fproc {

	class ImageWindow {
	public:
		ImageWindow() {}
		virtual ~ImageWindow() { if (!_win.is_closed()) _win.close_window(); }

		void show(PFrame frame);
		bool isClosed() {return _win.is_closed();}
	private:
		dlib::image_window _win;
	};

}



#endif /* SRC_VIDEO_STREAMING_IMAGE_WINDOW_HPP_ */
