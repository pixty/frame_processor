/*
 * fpcp_client.hpp
 *
 *  Created on: Aug 3, 2017
 *      Author: dmitry
 */

#ifndef SRC_FPCP_GRPC_FPCP_CLIENT_HPP_
#define SRC_FPCP_GRPC_FPCP_CLIENT_HPP_

#include "../model.hpp"

namespace fpcp {
namespace rpc {

// Scene Processor can throw the errors
enum Error {
	NONE = 0,
	AUTH_REQUIRED = 1,
	WRONG_CREDENTIALS = 2,
	TRY_AGAIN_LATER = 3,
	GRPC_FAILED, // connection with SP is failed or wire is broken
	PROTOCOL_ISSUE, // the expectations are not met
	UNKNOWN
};

struct SceneProcessor {
	virtual ~SceneProcessor(){}
	virtual void onScene(fproc::PScene pscene) {};
};
typedef std::shared_ptr<SceneProcessor> PSceneProcessor;

// Connects to a scene processor via gRPC. Call it like connect("localhost:50051");
// access and secret keys are for authentications
PSceneProcessor connectGRPC(std::string address, std::string access_key, std::string secret_key);

}
};

#endif /* SRC_FPCP_GRPC_FPCP_CLIENT_HPP_ */
