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
#include "../config_params.hpp"
#include "../frame_processing/image_processing.hpp"

namespace fpcp {
namespace rpc {

class FpcpClient: public SceneProcessor {
protected:
	FpcpClient(std::shared_ptr<grpc::Channel> channel, const fproc::FpcpClientParameters& params) :
			stub_(SceneProcessorService::NewStub(channel)), params_(params), session_id_("") {
		if (!fproc::ImageSize::isValidCode(params_.dflt_size_code)) {
			LOG_INFO("FpcpClient: Unknown default picture size code='" << params_.dflt_size_code << "' will use medium one 'm' (640x480) as default.");
			params_.dflt_size_code = fproc::ImageSize::Code::IMG_SZ_640x480;
		}

		LOG_INFO("FpcpClient: Initializing with default picture size=" << params_.dflt_size_code << ", compression will be " << (params_.jpeg ? "JPEG" : "PNG"));
	}

public:
	void onScene(fproc::PScene pscene);

private:
	void authenticate(grpc::ClientContext& context);
	void check_session();
	void check_response(grpc::Status& status, grpc::ClientContext& context, std::string method);

	fproc::ImageSize::Code defSizeCode() { return fproc::ImageSize::Code(params_.dflt_size_code); }
	void to_fpcpScene(fproc::PScene pscene, fpcp::Scene& scene);
	void to_fpcpFace(fproc::PFrame pframe, const fproc::FrameFace ff, fpcp::Face& face);
	void to_fpcpFrame(fproc::PFrame pframe, fpcp::Frame& frame);
	bool to_fpcpPicture(fproc::PFrame frame, const fproc::Rectangle* rect, fproc::ImageSize::Code saveAsCode, fproc::ImageSize::Code toCode, fpcp::Picture& pic);

	std::unique_ptr<SceneProcessorService::Stub> stub_;
	fproc::FpcpClientParameters params_;
	std::string session_id_;

	friend PSceneProcessor connectGRPC(const fproc::FpcpClientParameters& params);
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
	if (err == "3") {
		LOG_WARN("FpcpClient: SP returns - Unable. Try again later.");
		return Error::TRY_AGAIN_LATER;
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
	if (err == Error::AUTH_REQUIRED) {
		session_id_ = "";
		LOG_WARN("FpcpClient: gRPC call to \"" << method << "\" was successful, but SP returns AUTH_REQUIRED. Dropping current session_id_");
	}
	if (err != Error::NONE) {
		LOG_WARN("FpcpClient: gRPC call to \"" << method << "\" was successful, but SP returns the error in meta-data: err=" << err << ", current session_id=" << session_id_);
		throw err;
	}
}

// Transforms rectangle on the frame, or the whole frame to the picture object using toCode as a transformation code size and mark the pictur as saveAsCode
bool FpcpClient::to_fpcpPicture(fproc::PFrame frame, const fproc::Rectangle* rect, fproc::ImageSize::Code saveAsCode, fproc::ImageSize::Code toCode, fpcp::Picture& pic) {
	std::vector<uchar> res_buf;
	fproc::CompType comp_tp = fproc::CompType::PNG;
	int quality = 9;

	if (params_.jpeg) {
		comp_tp = fproc::CompType::JPEG;
		quality = 95;
	}

	fproc::Size size;
	if (rect) {
		size = fproc::compress_frame_region(frame, *rect, toCode, params_.cut_face_border, res_buf, comp_tp, quality);
	} else {
		size = fproc::compress_frame(frame, toCode, res_buf, comp_tp, quality);
	}
	if (size == fproc::ImageSize::ORIGINAL_SIZE) {
		const fproc::Rectangle nullRect = rect ? *rect : fproc::Rectangle();
		LOG_WARN("FpcpClient: Oops. Could not compress image using comt_tp=" << comp_tp << ", saveAsCode=" << saveAsCode << ", toCode=" << toCode
				<< " rect=" << nullRect);
		return false;
	}

	fpcp::Size* sz = pic.mutable_size();
	sz->set_height(size.height);
	sz->set_width(size.width);
	pic.set_data(&res_buf[0], res_buf.size());
	pic.set_sizecode(saveAsCode);
	pic.set_format(params_.jpeg ? fpcp::Picture_Format_JPG : fpcp::Picture_Format_PNG);
	return true;
}

void to_fpcpRect(const fproc::Rectangle& r, fpcp::Rectangle& rect) {
	rect.set_top(r.top());
	rect.set_left(r.left());
	rect.set_bottom(r.bottom());
	rect.set_right(r.right());
}

void FpcpClient::to_fpcpFrame(fproc::PFrame pframe, fpcp::Frame& frame) {
	frame.set_id(std::to_string(pframe->getId()));
	frame.set_timestamp(pframe->getTimestamp());

	// true frame size
	fpcp::Size* size = frame.mutable_size();
	fproc::Size sz = pframe->size();
	size->set_width(sz.width);
	size->set_height(sz.height);

	// We store here 2 pictures - one is to default size, another one on 1 size lesser
	fpcp::Picture *pp = frame.add_pictures();
	to_fpcpPicture(pframe, NULL, fproc::ImageSize::Code::ORIGINAL, defSizeCode(), *pp);
	fproc::ImageSize::Code smCode = fproc::ImageSize::smaller(defSizeCode());
	if (smCode != fproc::ImageSize::Code::ORIGINAL) {
		pp = frame.add_pictures();
		to_fpcpPicture(pframe, NULL, smCode, smCode, *pp);
	}
}

void FpcpClient::to_fpcpFace(fproc::PFrame pframe, const fproc::FrameFace ff, fpcp::Face& face) {
	face.set_id(ff.faceId());
	const fproc::Rectangle& rect = ff.frameReg()->getRectangle();
	to_fpcpRect(rect, *face.mutable_rect());
	// copy vector
	for(long i = 0; i < 128; i++) {
		float f = (*ff.frameReg()->v128d())(i);
		face.add_vector(f);
	}

	// Store pictures now
	fproc::Size osz = fproc::rectSize(rect);
	if (osz.area() <= 0) {
		LOG_WARN("FpcpClient: something wrong with face rectangle, its area is 0 or less " << rect);
		return;
	}

	// if the original face cut is bigger than default size, we use the default one, or will use original otherwise
	fproc::ImageSize::Code toCode = fproc::ImageSize::getMajor(osz) > defSizeCode() ? defSizeCode() : fproc::ImageSize::Code::ORIGINAL;
	fpcp::Picture *pp = face.add_pictures();
	to_fpcpPicture(pframe, &rect, fproc::ImageSize::Code::ORIGINAL, toCode, *pp);

	// will add smaller picture if required
	toCode = fproc::ImageSize::getMinorCode(osz);
	fproc::Size sz = fproc::ImageSize::getSize(toCode);
	if (float(sz.area())/osz.area() > 0.75) {
		// still not big difference, choose smaller
		toCode = fproc::ImageSize::smaller(toCode);
	}

	if (toCode != fproc::ImageSize::Code::ORIGINAL) {
		pp = face.add_pictures();
		to_fpcpPicture(pframe, &rect, toCode, toCode, *pp);
	}
}

void FpcpClient::to_fpcpScene(fproc::PScene pscene, fpcp::Scene& scene) {
	fproc::PFrame pf = pscene->getFrame();
	to_fpcpFrame(pf, *scene.mutable_frame());
	for (auto ff: *pscene->getFrameFaceList()) {
		fpcp::Face * face = scene.add_faces();
		to_fpcpFace(pf, ff, *face);
	}
	scene.set_id(pscene->getId());
	scene.set_since(pscene->getSince());
	scene.set_persons(pscene->getPersons());
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

	request.set_access(params_.access_key);
	request.set_secret(params_.secret_key);

	grpc::Status status = stub_->authenticate(&context, request, &response);
	check_response(status, context, "authenticate");
	session_id_ = getMetaVal(context.GetServerTrailingMetadata(), META_KEY_SESSION_ID);
	if (session_id_.length() == 0) {
		LOG_ERROR("FpcpClient: session_is is expected, but not returned even when gRPC call is ok. Failed in check_session()!");
		throw Error::PROTOCOL_ISSUE;
	}
	LOG_INFO("FpcpClient: authenticated successfully for access_key=" << params_.access_key << ", session_id=" << session_id_);
}

PSceneProcessor connectGRPC(const fproc::FpcpClientParameters& params) {
	LOG_INFO("connectGRPC: address=" << params.address << ", access_key=" << params.access_key);
	std::shared_ptr<grpc::Channel> channel = grpc::CreateChannel(params.address,
			grpc::InsecureChannelCredentials());
	return PSceneProcessor(new FpcpClient(channel, params));
}

}
}
