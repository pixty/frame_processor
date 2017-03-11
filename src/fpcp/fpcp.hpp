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

typedef std::string string;
typedef string id;
typedef int my_error;

struct FPCPReq {
	id reqId;
	bool scene;
	id imgId;
	id personId;
};

struct FPCPResp {
	id reqId;
	my_error error;
	fproc::Scene* scene;
	fproc::FrameRegion* image;
	fproc::Face* person;
};

struct SPRequestListener {
	virtual ~SPRequestListener() {}
	virtual void onFPCPReq(FPCPReq &req) = 0;
};
typedef std::shared_ptr<SPRequestListener> PSPRequestListener;

string to_json(const fproc::Scene& scene);
string to_json(const FPCPResp& resp);
void from_json(string json, FPCPReq& req);

}

#endif /* SRC_FPCP_HPP_ */
