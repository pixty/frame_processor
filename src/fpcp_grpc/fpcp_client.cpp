/*
 * fpcp_client.cpp
 *
 *  Created on: Aug 3, 2017
 *      Author: dmitry
 */

#include "fpcp_client.hpp"
#include <grpc++/grpc++.h>
#include "fpcp.grpc.pb.h"
#include "../logger.hpp"

namespace fpcp {
namespace rpc {

class FpcpClient: public SceneProcessor {
protected:
	FpcpClient(std::shared_ptr<grpc::Channel> channel, std::string access_key, std::string secret_key) :
			stub_(SceneProcessorService::NewStub(channel)), access_key_(access_key), secret_key_(secret_key), session_id_("") {
	}

public:
	void onScene(fproc::PScene pscene);

private:
	void authenticate(grpc::ClientContext& context);
	void check_session();
	void check_response(grpc::Status& status, grpc::ClientContext& context, std::string method);

	std::unique_ptr<SceneProcessorService::Stub> stub_;
	std::string access_key_;
	std::string secret_key_;
	std::string session_id_;

	friend PSceneProcessor connectGRPC(std::string address, std::string access_key, std::string secret_key);
};

typedef std::multimap<grpc::string_ref, grpc::string_ref> ServerMeta;

static const std::string META_KEY_ERROR = "error";
static const std::string META_KEY_SESSION_ID = "session_id";

std::string getMetaVal(const ServerMeta& meta, const std::string& key) {
	ServerMeta::const_iterator val = meta.find(key);
	if (val == meta.end()) {
		return "";
	}
	grpc::string_ref sr = val->second;
	std::string res(sr.data(), sr.size());
	return res;
}

Error getErrorVal(const grpc::ClientContext& context) {
	std::string err = getMetaVal(context.GetServerTrailingMetadata(), META_KEY_ERROR);
	if (!err.length()) {
		return Error::NONE;
	}
	if (err == "1") {
		LOG_WARN("FpcpClient: SP returns - Authentication required.");
		return Error::AUTH_REQUIRED;
	}
	if (err == "2") {
		LOG_WARN("FpcpClient: SP returns - Wrong credentials.");
		return Error::WRONG_CREDENTIALS;
	}
	LOG_WARN("FpcpClient: SP returns error=\"" << err << "\"");
	return Error::UNKNOWN;
}

void FpcpClient::check_response(grpc::Status& status, grpc::ClientContext& context, std::string method) {
	if (!status.ok()) {
		LOG_WARN("FpcpClient: gRPC call to \"" << method << "\" is failed statusCode=" << status.error_code() << ": " << status.error_message());
		throw Error::GRPC_FAILED;
	}

	Error err = getErrorVal(context);
	if (err != Error::NONE) {
		LOG_WARN("FpcpClient: gRPC call to \"" << method << "\" was successful, but SP returns the error in meta-data: err=" << err << ", current session_id=" << session_id_);
		throw err;
	}
}

void to_fpcpRect(const fproc::Rectangle& r, fpcp::Rectangle& rect) {
	rect.set_top(r.top());
	rect.set_left(r.left());
	rect.set_bottom(r.bottom());
	rect.set_right(r.right());
}

void to_fpcpFrame(fproc::PFrame pframe, fpcp::Frame& frame) {
	frame.set_id(std::to_string(pframe->getId()));
	frame.set_timestamp(pframe->getTimestamp());

	fpcp::Size* size = frame.mutable_size();
	size->set_width(pframe->get_mat().cols);
	size->set_height(pframe->get_mat().rows);

	std::vector<uchar>& ubuf = pframe->png_buf();
	frame.set_data(&ubuf[0], ubuf.size());

	frame.set_format(Frame_Format::Frame_Format_PNG);
}

void to_fpcpFace(const fproc::FrameFace ff, fpcp::Face& face) {
	face.set_id(ff.faceId());
	to_fpcpRect(ff.frameReg()->getRectangle(), *face.mutable_rect());
}

void to_fpcpScene(fproc::PScene pscene, fpcp::Scene& scene) {
	to_fpcpFrame(pscene->getFrame(), *scene.mutable_frame());
	for (auto ff: *pscene->getFrameFaceList()) {
		fpcp::Face * face = scene.add_faces();
		to_fpcpFace(ff, *face);
	}
}

void FpcpClient::onScene(fproc::PScene pscene) {
	check_session();

	fpcp::Scene request;
	to_fpcpScene(pscene, request);

	fpcp::Void response;
	grpc::ClientContext context;
	context.AddMetadata(META_KEY_SESSION_ID, session_id_);

	// The actual RPC.
	grpc::Status status = stub_->onScene(&context, request, &response);
	check_response(status, context, "onScene");
}

void FpcpClient::check_session() {
	if (session_id_.length()) {
		return;
	}

	fpcp::AuthToken request;
	fpcp::Void response;
	grpc::ClientContext context;

	request.set_access(access_key_);
	request.set_secret(secret_key_);

	grpc::Status status = stub_->authenticate(&context, request, &response);
	check_response(status, context, "authenticate");
	session_id_ = getMetaVal(context.GetServerTrailingMetadata(), META_KEY_SESSION_ID);
	if (session_id_.length() == 0) {
		LOG_ERROR("FpcpClient: session_is is expected, but not returned even when gRPC call is ok. Failed in check_session()!");
		throw Error::PROTOCOL_ISSUE;
	}
	LOG_INFO("FpcpClient: authenticated successfully for access_key=" << access_key_ << ", session_id=" << session_id_);
}

PSceneProcessor connectGRPC(std::string address, std::string access_key, std::string secret_key) {
	std::shared_ptr<grpc::Channel> channel = grpc::CreateChannel(address,
			grpc::InsecureChannelCredentials());
	return PSceneProcessor(new FpcpClient(channel, access_key, secret_key));
}

}
}
