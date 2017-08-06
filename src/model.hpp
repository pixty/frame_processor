/*
 * model.hpp
 *
 *  Created on: Jan 24, 2017
 *      Author: dmitry
 */

#ifndef SRC_MODEL_HPP_
#define SRC_MODEL_HPP_

#include <list>

#include <boost/thread/lockable_concepts.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>

#include <dlib/geometry/rectangle.h>
#include <dlib/opencv.h>

#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>

namespace fproc {

	/*
	 * Just a Rectangle. Let's use dlib one for beginning.
	 */
	typedef dlib::rectangle Rectangle;
	typedef cv::Size Size;
	typedef cv::Rect CvRect;

	/*
	 * Face descriptor in 128-dimensional space
	 */
	typedef dlib::matrix<float,0,1> V128D;
	float distance(V128D &v1, V128D &v2);

	Rectangle toRectangle(const cv::Rect2d& cv_rect);
	Rectangle toRectangle(const CvRect& cv_rect);
	CvRect toCvRect(const Rectangle& rect);

	typedef std::shared_ptr<boost::thread> PThread;
	typedef boost::unique_lock<boost::mutex> MxGuard;

	/*
	 * Timestamp is a type which describes a timestamp in millis. We can define
	 * some methods of transformation and comparison later, lets use long as a standard holder
	 * for millis since 01.01.1970
	 */
	typedef long Timestamp;

	constexpr static Timestamp NoTimestamp = -1;

	/*
	 * Returns current timestamp
	 */
	Timestamp ts_now();

	/*
	 * A Frame identifier. All Frames provided by a VideoStream are unique and the number is gradually
	 * increased in the time. So frames with low number is older than frames with high one.
	 * Different VideoStreams can produce frames with the same id
	 */
	typedef long FrameId;

	/*
	 * A face identifier. The identifier is assigned by a SceneDetector when a scene is build. The
	 * identifier is intended for distinguish persons. Same person always has the same FaceId. Different
	 * persons always have different FaceIds.
	 */
	typedef std::string FaceId;
	typedef std::list<FaceId> FaceList;

	/*
	 * Frame class describes a video snapshot (picture). The class contains some internal structures
	 * for representing a frame or a reference to a file if the picture was stored on disk.
	 *
	 * Every frame has its own FrameId which uniquely identifies the frame in the video stream. Timestamp
	 * indicates the time when the Frame was taken.
	 */
	class Frame {
	public:
		/*
		 * Different representations of a frame
		 */
		typedef cv::Mat CvBgrMat;
		typedef dlib::cv_image<dlib::bgr_pixel> DlibBgrImg;
		typedef dlib::matrix<dlib::rgb_pixel> DlibRgbImg;
	private:
		typedef std::unique_ptr<DlibBgrImg> bgr_image;
		typedef std::unique_ptr<DlibRgbImg> rgb_image;
	public:

		Frame(FrameId id, Timestamp ts): _id(id), _ts(ts) {}
		/*
		 * Unique frame identifier. Gradually increased, so can be used for comparison.
		 */
		const FrameId getId() const { return _id;}

		/*
		 * The frame timestamp, contains timestamp when the frame was taken.
		 */
		const Timestamp getTimestamp() const { return _ts; }

		bool isEmpty() const { return _mat.size().width < 1; }

		DlibBgrImg& get_bgr_image();
		DlibRgbImg& get_rgb_image();
		CvBgrMat& get_mat() { return _mat; }

	private:
		FrameId _id;
		Timestamp _ts;
		CvBgrMat _mat;
		bgr_image _bgr_img;
		rgb_image _rgb_img;
	};
	typedef std::shared_ptr<Frame> PFrame;

	/*
	 * FrameRegion - describes a region in a frame. Used for describing objects
	 * in a frame. Always has a non-NULL frame because it connects to it.
	 */
	struct FrameRegion {
		FrameRegion(PFrame pFrame, const Rectangle& rec) : _frame(pFrame), _rec(rec) {}
		const PFrame getFrame() const { return _frame; }
		const Rectangle& getRectangle() const { return _rec; }
		V128D& v128d() {return _v128d;};
		void set_vector(const V128D& v) { _v128d = v;};

	private:
		const PFrame _frame;
		const Rectangle _rec;
		V128D _v128d;
	};
	typedef std::shared_ptr<FrameRegion> PFrameRegion;
	typedef std::list<PFrameRegion> PFrameRegList;

	// A Face associated with a single frame only
	struct FrameFace {
		FrameFace(FaceId fid, const PFrameRegion fr): face_id_(fid), frame_reg_(fr) { };
		FrameFace& operator=(const FrameFace& othr) {
			face_id_ = othr.face_id_;
			frame_reg_ = othr.frame_reg_;
			return *this;
		}
		bool operator<(const FrameFace& othr) const { return face_id_ < othr.face_id_;}
		const FaceId& faceId() const { return face_id_; }
		const PFrameRegion frameReg() const { return frame_reg_; }
	private:
		FaceId face_id_;
		PFrameRegion frame_reg_;
	};
	typedef std::list<const FrameFace> FrameFaceList;

	/*
	 * The ObjectDetector detects objects in a frame and returns their regions in the frame
	 */
	struct ObjectDetector {
		virtual ~ObjectDetector() {}
		virtual const PFrameRegList& detectRegions(PFrame pFrame) = 0;
		virtual const PFrameRegList& detectRegions(PFrame pFrame, const std::vector<Rectangle>& suggested_rects) { return _objects; }
	protected:
		// helper to return detected objects
		PFrameRegList _objects;
	};

	/*
	 * VideoStream is an interface which defines basic methods for a video stream
	 * providers.
	 */
	struct VideoStream {
		/*
		 * Returns next video frame. It can return NULL if the VideoStream is closed or
		 * over (depends on source and implementation)
		 */
		virtual PFrame captureFrame() = 0;
		virtual ~VideoStream() {};

		void setResolution(int width, int height);
		Size getSize();
		double getFps() { return _cap->get(CV_CAP_PROP_FPS); };
		int getFourcc() { return static_cast<int>(_cap->get(CV_CAP_PROP_FOURCC)); };
	protected:
		VideoStream(std::unique_ptr<cv::VideoCapture> cap): _cap(std::move(cap)) { }
		std::unique_ptr<cv::VideoCapture> _cap;
	};
	typedef std::unique_ptr<VideoStream> PVideoStream;

	/*
	 * VideoStreamConsumer is a helper interface for copying frames strategy
	 */
	struct VideoStreamConsumer {
		VideoStreamConsumer() {}
		virtual ~VideoStreamConsumer() {}

		virtual bool consumeFrame(PFrame frame) { return true; };
		virtual void close() {};
	};
	typedef std::unique_ptr<VideoStreamConsumer> PVideoStreamConsumer;

	/**
	 * The VideoStreamCopier just copies frames from src to dst
	 */
	struct VideoStreamCopier {
		VideoStreamCopier(PVideoStream& src, PVideoStreamConsumer& dst):_src(std::move(src)), _dst(std::move(dst)), _started(false) {}

		// returns whether the processing was normally over(true) or stopped by explicit stop() call(false)
		bool process();
		void stop();

	private:
		PVideoStream _src;
		PVideoStreamConsumer _dst;
		bool _started;
		boost::mutex _lock;
	};

	std::string uuid();
};
// namespace

#endif /* SRC_MODEL_HPP_ */
