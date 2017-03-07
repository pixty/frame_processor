/*
 * fpcp_http.hpp
 *
 *  Created on: Mar 6, 2017
 *      Author: dmitry
 */

#ifndef SRC_FPCP_FPCP_HTTP_HPP_
#define SRC_FPCP_FPCP_HTTP_HPP_

#import "fpcp.hpp"

namespace fpcp {

class FprocEndHttp {
public:
	FprocEndHttp(id fpId, std::string url, SPRequestListener &listener):_id(fpId), _url(url), _listener(listener) {
		_long_poll_to_sec = 0;
	}
	virtual ~FprocEndHttp() {}

	void sendScene(const fproc::Scene &scene);

private:
	id _id;
	std::string _url;
	SPRequestListener &_listener;
	int _long_poll_to_sec;
};

}



#endif /* SRC_FPCP_FPCP_HTTP_HPP_ */
