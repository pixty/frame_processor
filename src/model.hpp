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

	/*
	 * Timestamp is a type which describes a timestamp in millis. We can define
	 * some methods of transformation and comparison later, lets use long as a standard holder
	 * for millis since 01.01.1970
	 */
	typedef long Timestamp;

	/*
	 * Returns current timestamp
	 */
	Timestamp ts_now();

	/*
	 * A Frame identifier. All Frames provided by a VideoStream are unique and the number is gradually
	 * increased in the time. So frames with low number is older than frames with high one.
	 */
	typedef long FrameId;

	/*
	 * A face identifier. The identifier is assigned by a SceneDetector when a scene is build. The
	 * identifier is intended for distinguish persons. Same person always has the same FaceId. Different
	 * persons always have different FaceIds.
	 */
	typedef std::string FaceId;

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
		 * Different represenations of a frame
		 */
		typedef cv::Mat CvBgrMat;
		typedef dlib::cv_image<dlib::bgr_pixel> DlibBgrImg;
	private:	  
		typedef std::unique_ptr<DlibBgrImg> pcv_image;		
	public:

		Frame(FrameId id, Timestamp ts): _id(id), _ts(ts) {}
		/*
		 * Unique frame identifier. Gradually increased, so can be used for comparison.
		 * QUESTION - I am not sure we need it, cause Timestamp can be used instead. The only
		 * reason why we can have it is uniqueness, cause in theory 2 and more frames can have the same
		 * timestamp, but we need to distinguish them somehow..
		 */
		const FrameId getId() const { return _id; }

		/*
		 * The frame timestamp, contains timestamp when the frame was taken.
		 */
		const Timestamp getTimestamp() const { return _ts; }

		DlibBgrImg& get_cv_image();
		CvBgrMat& get_mat() { return _mat; }
	private:
		FrameId _id;
		Timestamp _ts;
		CvBgrMat _mat;
		pcv_image _cv_img;
	};
	
	typedef std::shared_ptr<Frame> PFrame;

	/*
	 * FrameRegion - describes a region in a frame. Used for describing objects
	 * in a frame. Always has a non-NULL frame because it connects to it.
	 */
	class FrameRegion {
	public:
		FrameRegion(PFrame pFrame, Rectangle rec): _frame(pFrame), _rec(rec) {}

		PFrame getFrame() const { return _frame; }
		Rectangle& getRectangle() { return _rec; }
	private:
		PFrame _frame;
		Rectangle _rec;
	};
	typedef std::list<FrameRegion> FRList;

	/*
	 * The ObjectDetector detects objects in a frame and returns their regions in the frame
	 */
	struct ObjectDetector {
		virtual ~ObjectDetector() {}
		virtual FRList& detectRegions(PFrame pFrame) = 0;
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
		virtual ~VideoStream() { };

		Size getSize();
		double getFps() { return _cap->get(CV_CAP_PROP_FPS); };
		int getFourcc() { return static_cast<int>(_cap->get(CV_CAP_PROP_FOURCC));};
	protected:
		VideoStream(std::unique_ptr<cv::VideoCapture> cap): _cap(std::move(cap)) {}

		std::unique_ptr<cv::VideoCapture> _cap;
	};

	/*
	 * A Face description. An immutable object which is built for describing a scene. It keeps a list
	 * of images (frame regions) where the face was detected. Not all frames can be included into
	 * the list, but only some images that can be helpful for further processing (good quality etc.)
	 *
	 * Same Faces are ALWAYS reported with same FaceId. It is SceneDetector responsibility to distinguish
	 * faces, select the list of images and defines a strategy why it (SceneDetector) decides that
	 * the images are for the same face.
	 */
	struct Face {
		Face(const FaceId id, const Timestamp firstTimeCatched):
		      _id(id),
		      _firstTimeCatched(firstTimeCatched),
		      _lostTime(-1) {}
		
		std::list<FrameRegion>& getImages(){return _regions;};
		const FaceId getId() const {return _id;};
		const Timestamp firstTimeCatched() const {return _firstTimeCatched;};
		const Timestamp lostTime() const {return _lostTime;};
		void setLostTime(const Timestamp lostTime){_lostTime = lostTime;}
		/* Other methods and members are not defined yet */
	private:
		const FaceId _id;
		const Timestamp _firstTimeCatched;
		Timestamp _lostTime;
		std::list<FrameRegion> _regions;
	};

	 typedef std::shared_ptr<Face> PFace;

	/*
	 * Scene is a cognitive description (or semantic) what is going on in the VideoStream at a moment.
	 * The scene object is built by SceneDetector and it is a result of some frame processing and
	 * conclusions made by the SceneDetector implementation logic.
	 */
	struct Scene {
		Scene(Timestamp since): _since(since) {}
		Scene(Timestamp since, std::list<PFace>& faces) :_since(since), _faces(faces) {}

		// Returns list of faces, who are on the scene right now
		std::list<PFace> getFaces() const {return _faces; }

		// Returns timestamp when the scene forms. Actually it is a moment when the
		// SceneDetector "built" the faces list first time.
		inline Timestamp since() const { return _since; };

	private:
		Timestamp _since;
		std::list<PFace> _faces;
	};

	/*
	 * An interface which defines notifications that SceneDetector implementation can call
	 * during video stream processing.
	 */
	struct SceneDetectorListener {
		virtual void onSceneChanged(Scene& scene) {};
		virtual void onStopped() {};
		virtual ~SceneDetectorListener() {}
	};
	extern SceneDetectorListener nil_sc_detecor_listener;

	/*
	 * SceneDetector is an interface which defines a scene detector life-cycle. The SceneDetector
	 * itself intends for processing a video stream frames, to handle them and eventually detecting
	 * the current scene state.
	 *
	 * The normal usage of the detector is to call the process() method, which will block the invoker
	 * thread until an unrecoverable error (exception) happens, or the stop() method is called by
	 * another thread.
	 */
	struct SceneDetector {
		typedef std::shared_ptr<Scene> PScene;	
		
		SceneDetector(VideoStream& vstream, SceneDetectorListener& listener);
		// Returns the scene state
		const PScene& getScene() const { return _scene; }
		void process();
		void stop();

		virtual ~SceneDetector() {}

	protected:
		virtual void doProcess(PFrame frame)=0;
		virtual void onStop() {}

		VideoStream& _vstream;
		SceneDetectorListener& _listener;
		PScene _scene;
		bool _started;
		boost::mutex _lock;
	};

};// namespace

#endif /* SRC_MODEL_HPP_ */
