/*
 * file_video_stream.hpp
 *
 *  Created on: Jan 25, 2017
 *      Author: dmitry
 */

#ifndef SRC_VIDEO_STREAMING_FILE_VIDEO_STREAM_HPP_
#define SRC_VIDEO_STREAMING_FILE_VIDEO_STREAM_HPP_

#include "../model.hpp"

namespace fproc {

	struct FileVideoStreamCfg {
	  constexpr static int NONE_FRAME_NUM = -1;
	  FileVideoStreamCfg():FileVideoStreamCfg(std::string()){}
	  FileVideoStreamCfg(const std::string filename, 
			     const int startFrom = NONE_FRAME_NUM, 
			     const int stopAfter=NONE_FRAME_NUM,
			     const bool cycling = false):
			    _filename(filename),
			    _startFrom(startFrom),
			    _stopAfter(stopAfter),
			    _cycling(cycling){}
			    
	  const std::string filename() const { return _filename;}
	  const int startFrom()const{return _startFrom;}
	  const int stopAfter()const { return _stopAfter;}
	  const bool cycling() const { return _cycling;}
	  
	  void filename(const std::string &filename){_filename = filename;}
	  void startFrom(const int startFrom){_startFrom = startFrom;}
	  void stopAfter(const int stopAfter){_stopAfter = stopAfter;}
	  void cycling(const bool cycling){_cycling = cycling;}

	private:
	  std::string _filename;
	  int _startFrom;
	  int _stopAfter;
	  bool _cycling;
	};

	/*
	 * FileVideoStream - a video stream captured from a file.
	 */
	class FileVideoStream: public VideoStream {
	public:
		/*
		 * Constructs new video stream. The "cycling" param allows to play the video in a loop non-stop
		 */
		FileVideoStream(const FileVideoStreamCfg &cfg);
		virtual ~FileVideoStream() {}

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

		inline std::string& getFileName() { return _filename; }

	private:
		void skip();
		Timestamp _started_at;
		std::string _filename;
		int _startFrom;
		int _stopAfter;
		bool _skipped;
	};

	class FileVStreamWriter {
	public:
		FileVStreamWriter(const std::string& fileName, int fourcc, double fps,
                Size frameSize);
		virtual ~FileVStreamWriter() {}

		void write(PFrame);
		void close() { _writer->release(); }
	private:
		std::unique_ptr<cv::VideoWriter> _writer;
	};
}


#endif /* SRC_VIDEO_STREAMING_FILE_VIDEO_STREAM_HPP_ */
