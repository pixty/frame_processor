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
typedef int error;

constexpr static char const* nullId = "";

inline bool is_empty(id id) {
	return id == nullId;
}

struct FPCPReq {
	FPCPReq(): reqId(""), scene(false), imgId(""), personId("") {}
	id reqId;
	bool scene;
	id imgId;
	id personId;
};

struct FPCPResp {
	FPCPResp(): reqId(""), error(0), scene(NULL), image(NULL), person(NULL) {}
	id reqId;
	error error;
	fproc::Scene* scene;
	fproc::FrameRegion* image;
	fproc::Face* person;
};

struct SPRequestListener {
	virtual ~SPRequestListener() {}

	virtual void onSceneRequest() = 0;
	virtual void onImageRequest(id reqId, id imgId) = 0;
	virtual void onPersonRequest(id reqId, id personId) = 0;
};
typedef std::shared_ptr<SPRequestListener> PSPRequestListener;

string to_json(const fproc::Scene& scene);
string to_json(const FPCPResp& resp);
void from_json(string json, FPCPReq& req);

}

#endif /* SRC_FPCP_HPP_ */
