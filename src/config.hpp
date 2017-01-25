#ifndef SRC_CONFIG_HPP_
#define SRC_CONFIG_HPP_

#include <string>

namespace fp {

    class Config{
    public:
        class SourceConfig{
        public:
            SourceConfig();

            int getWidth()const {return _width;}
            void setWidth(const int width){_width = width;}

            int getHeight()const {return _height;}
            void setHeight(const int height){_height = height;}

            const std::string& getUri()const {return _uri;}
            void setUri(std::string uri){_uri = uri;}

        private:
            int _width;
            int _height;
            // as specified by cv::VideoCapture::open(), empty string means the first available camera
            std::string _uri;
        };

        class ProcessorConfig{
        public:
            ProcessorConfig();

            int getFrom() const {return _from;}
            void setFrom(const int from){_from = from;}

            int getTotal() const {return _total;}
            void setTotal(const int total) {_total = total;}

        private:
            int _from;
            int _total;
        };

        class Mog2Config{
        public:
            Mog2Config();

            inline double getLearningRate() const {return _learningRate;}
            void setLearningRate(const double learnRate){_learningRate = learnRate;}

            inline int getHistory() const {return _history;}
            void setHistory(const int history){_history = history;}

            inline int getThreshold() const {return _threshold;}
            void setThreshold(const int threshold){_threshold = threshold;}

            inline bool getShadow() const {return _shadow;}
            void setShadow(const bool shadow){_shadow = shadow;}

        private:
            double _learningRate;
            int _history;
            int _threshold;
            bool _shadow;
        };

        Config(){};

        const Mog2Config& getMog2Config(){return _mog2Config;}

        Mog2Config& mog2Config(){return _mog2Config;}

        const SourceConfig& getSourceConfig(){return _sourceConfig;}

        SourceConfig& sourceConfig(){return _sourceConfig;}

        const ProcessorConfig& getProcessorConfig(){return _processorConfig;}

        ProcessorConfig& processorConfig(){return _processorConfig;}

        static const Config & parse(const std::string &fileName);

    private:
        SourceConfig _sourceConfig;

        Mog2Config _mog2Config;

        ProcessorConfig _processorConfig;
    };
}
#endif // SRC_CONFIG_HPP_
