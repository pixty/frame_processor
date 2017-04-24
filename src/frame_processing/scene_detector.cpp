#include "../model.hpp"
#include "../logger.hpp"
#include <boost/thread/locks.hpp>

namespace fproc {

SceneDetector::SceneDetector(PVideoStream vstream, PSceneDetectorListener listener):
    _vstream(std::move(vstream)),
    _listener(std::move(listener)),
    _started(false)
{
}

void SceneDetector::process() {
	{
        boost::lock_guard<boost::mutex> guard(_lock);
		_started = true;
	}

	LOG_INFO("SceneDetector: Entering processing.");
	_listener->onStarted();
	try {
		while (_started) {
			PFrame frame = _vstream->captureFrame();
            if (frame.get()->get_mat().size().width < 1) {
                LOG_INFO("SceneDetector: got an empty frame. Stop processing");
				stop();
				break;
			}

			doProcess(frame);
		}
	} catch (std::exception &e) {
        LOG_ERROR("SceneDetector: Oops an exception happens: " << e.what());
		stop(); // just in case
	}
	LOG_INFO("SceneDetector: Exit processing.");
}

void SceneDetector::stop() {
    boost::lock_guard<boost::mutex> guard(_lock);
	if (!_started) {
		return;
	}
	LOG_INFO("SceneDetector: stop()");
	_started = false;
	_listener->onStopped();
	onStop();
}
}
