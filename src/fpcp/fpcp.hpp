/*
 * fpcp.hpp
 *
 *  Created on: Mar 5, 2017
 *      Author: dmitry
 */

#ifndef SRC_FPCP_HPP_
#define SRC_FPCP_HPP_

#include "../model.hpp"
#include "../logger.hpp"

namespace fpcp {

typedef std::string id;
typedef int error;

struct FPCPReq {
	id reqId;
	bool scene;
	id imgId;
	id personId;
};

struct FPCPResp {
	id reqId;
	error error;
	fproc::Scene* scene;
};

struct SPRequestListener {
	virtual ~SPRequestListener() {}
	virtual void onFPCPReq(FPCPReq &req) = 0;
};
typedef std::shared_ptr<SPRequestListener> PSPRequestListener;

class FprocEndHttp {
public:
	FprocEndHttp(SPRequestListener &listener):_listener(listener) {}
	virtual ~FprocEndHttp() {}

	void sendScene(const fproc::Scene &scene);

private:
	SPRequestListener &_listener;
};

}


#endif /* SRC_FPCP_HPP_ */
