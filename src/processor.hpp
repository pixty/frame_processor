#ifndef SRC_PROCESSOR_HPP_
#define SRC_PROCESSOR_HPP_

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/video/background_segm.hpp>

#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing.h>
#include <dlib/geometry/rectangle.h>
#include <dlib/geometry/drectangle.h>



#include "simple_profiler.hpp"
#include "config.hpp"

namespace fp {
    class Grayscaler{
    public:
        Grayscaler():_profiler("Grayscaler"){
        }

        inline void apply(const cv::Mat& original){
            _profiler.enter();
            cv::cvtColor(original, _frame, CV_BGR2GRAY);
            _profiler.exit();
        }

        inline const cv::Mat& frame(){
            return _frame;
        }

        private:
        cv::Mat _frame;
        SimpleProfiler _profiler;
    };

    class BackgrgoundSubstractor{
    public:

        BackgrgoundSubstractor(const Config::Mog2Config &mog2Config):
            _mask(),
            _learningRate(mog2Config.getLearningRate()),
            _profiler("BackgrgoundSubstractor"){
            _bs = cv::createBackgroundSubtractorMOG2(mog2Config.getHistory(), mog2Config.getThreshold(), mog2Config.getShadow());
        }

        inline void apply(const cv::Mat& original){
            _profiler.enter();
            _bs->apply(original, _mask, _learningRate);
            _profiler.exit();
        }

        inline const cv::Mat& mask(){
            return _mask;
        }
    private:
        cv::Mat _mask;
        cv::Ptr<cv::BackgroundSubtractor> _bs;
        SimpleProfiler _profiler;
        double _learningRate;
    };

    class FaceDetector{
    public:
        FaceDetector():_profiler("FaceDetector"){
            _detector = dlib::get_frontal_face_detector();
        }

        template <typename image_type>
        inline void apply(const image_type& original){
            _profiler.enter();
            _faces = _detector(original);
            _profiler.exit();
        }

        inline const std::vector<dlib::rectangle>& faces(){
            return _faces;
        }
    private:
        std::vector<dlib::rectangle> _faces;
        dlib::frontal_face_detector _detector;
        SimpleProfiler _profiler;
    };

    class CorrelationTracker{

    public:
        CorrelationTracker():
            _profiler("CorrelationTracker")
        {
            _tracker = new dlib::correlation_tracker();
        }

        virtual ~CorrelationTracker(){}

        template <typename image_type>
        inline void start(const image_type& img, const dlib::drectangle& r){
            if(_initial_rect.is_empty()){//TODO fix me
                _initial_rect = r;
                _tracker->start_track(img, _initial_rect);
            }
        }

        template <typename image_type>
        inline void apply(const image_type& img){
            if(_initial_rect.is_empty()){return;} // TODO fix me
            _profiler.enter();
            _tracker->update(img);
            _profiler.exit();
        }

        inline const dlib::drectangle position(){
            return _initial_rect.is_empty() ? _initial_rect : _tracker->get_position();//TODO fix me
        }

    private:
        cv::Ptr<dlib::correlation_tracker> _tracker;
        SimpleProfiler _profiler;
        dlib::drectangle _initial_rect;

    };


}

#endif // SRC_PROCESSOR_HPP_
