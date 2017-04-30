/*
 * fpcp_http.hpp
 *
 *  Created on: Mar 6, 2017
 *      Author: dmitry
 */

#ifndef SRC_FPCP_FPCP_HTTP_HPP_
#define SRC_FPCP_FPCP_HTTP_HPP_

#import "fpcp.hpp"
#include <boost/thread/thread.hpp>

namespace fpcp {

class FprocEndHttp {
	typedef std::shared_ptr<boost::thread> PThread;
	typedef boost::lock_guard<boost::mutex> MxGuard;
public:
	FprocEndHttp(id fpId, std::string url):_id(fpId), _url(url) {
		_started = false;
		_listener = NULL;
		_get_timeout = 0;
	}
	virtual ~FprocEndHttp() {}

	void withGetTimeout(int get_timeout) { _get_timeout = get_timeout;}

	void start(SPRequestListener* listener);
	void stop();

	void sendScene(id reqId, const fproc::Scene& scene);
	void sendImage(id reqId, const fproc::FrameRegion& frameReg);
	void sendPerson(id reqId, const fproc::Face& face);
	void sendError(id reqId, error error);

	// Synchronous call (HTTP POST) to server
	void postResponse(FPCPResp& resp);

private:
	void getF();

	id _id;
	std::string _url;
	SPRequestListener* _listener;
	volatile bool _started;

	// Specifies how long the GET request can be hold by server (in seconds) when
	// long polling happens
	int _get_timeout;

	PThread _thread;
	boost::mutex _lock;
};

}



#endif /* SRC_FPCP_FPCP_HTTP_HPP_ */
