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


// TODO: rename the namespace later
namespace fp_test {

	/*
	 * Just a Rectangle. Let's use dlib one for beginning.
	 */
	typedef dlib::rectangle Rectangle;

	/*
	 * Timestamp is a type which describes a timestamp in millis. We can define
	 * some methods of transformation and comparison later, lets use long as a standard holder
	 * for millis since 01.01.1970
	 */
	typedef long Timestamp;

	/*
	 * Returns current timestamp
	 */
	Timestamp Now();

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
		 * Unique frame identifier. Gradually increased, so can be used for comparison.
		 * QUESTION - I am not sure we need it, cause Timestamp can be used instead. The only
		 * reason why we can have it is uniqueness, cause in theory 2 and more frames can have the same
		 * timestamp, but we need to distinguish them somehow..
		 */
		const FrameId getId() const;

		/*
		 * The frame timestamp, contains timestamp when the frame was taken.
		 */
		const Timestamp getTimestamp() const;

		/* Other methods and members are not defined yet */
	};
	typedef std::unique_ptr<Frame> PFrame;

	/*
	 * FrameRegion - describes a region in a frame. Used for describing objects
	 * in a frame. Always has a non-NULL frame because it connects to it.
	 */
	class FrameRegion {
	public:
		PFrame getFrame();
		Rectangle getRectangle();
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
	};

	/*
	 * WebcamVideoStream - a video stream captured from a build-in camera
	 */
	class WebcamVideoStream: public VideoStream {
	public:
		virtual PFrame captureFrame();

		/* Other methods and members are not defined yet */
	};

	/*
	 * FileVideoStream - a video stream captured from a file.
	 */
	class FileVideoStream: public VideoStream {
	public:
		/*
		 * Constructs new video stream. The cycling param allows to play the video in a loop non-stop
		 */
		FileVideoStream(std::string fileName, bool cycling);
		virtual ~FileVideoStream();

		/*
		 * Captures a frame from the video file. The following conventions is used:
		 * - Frames timestamp is a timestamp when the video starts to play plus duration of
		 * the video to the moment of the mehtod call.
		 *
		 * Throws exception if file not found or cannot be properly decoded.
		 *
		 * Returns NULL if the stream is over and cycling == false
		 */
		virtual PFrame captureFrame();

		/* Other methods and members are not defined yet */
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
		std::list<FrameRegion> getImages();
		FaceId getId();
		Timestamp firstTimeCatched();

		/* Other methods and members are not defined yet */
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
		virtual void onSceneChanged(Scene& scene) = 0;
		virtual void onStopped() = 0;
		virtual ~SceneDetectorListener() {}
	};

	/*
	 * a nil implementation of SceneDetectorListener
	 */
	struct NilSceneDetectorListener: SceneDetectorListener {
		void onSceneChanged(Scene& scene) {}
		void onStopped() {}
	};

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
		SceneDetector(VideoStream& vstream, SceneDetectorListener& listener);
		// Returns the scene state
        const Scene& getScene() const { return _scene; }
		void process();
		void stop();

		virtual ~SceneDetector() {}

	protected:
        virtual void doProcess(PFrame &frame)=0;

		VideoStream& _vstream;
		SceneDetectorListener& _listener;
		Scene _scene;
		bool _started;
        boost::mutex _lock;
	};

	/*
	 * DefaultSceneDetector is a default implementation of the SceneDetector. It accepts the detector
	 * listener, what allows to have a scene-change notification mechanism.
	 */
	class DefaultSceneDetector: public SceneDetector {
	public:
		DefaultSceneDetector(VideoStream& vstream);
		DefaultSceneDetector(VideoStream& vstream, SceneDetectorListener& listener);

	protected:
        void doProcess(PFrame &frame);
	};

};
// namespace

#endif /* SRC_MODEL_HPP_ */
