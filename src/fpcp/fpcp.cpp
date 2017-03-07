/*
 * fpcp.cpp
 *
 *  Created on: Mar 5, 2017
 *      Author: dmitry
 */
#include "fpcp.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace pt = boost::property_tree;

namespace fpcp {

void to_json(const fproc::Scene& scene, pt::ptree &root);
void to_json(const fproc::PFList &faces, pt::ptree &root);
void to_json(const fproc::PFace face, pt::ptree &root);
void to_json(const fproc::FRList &frame_regions, pt::ptree &root);
void to_json(const fproc::FrameRegion &fregion, pt::ptree &root);
void to_json(const fproc::Rectangle &r, pt::ptree &root);

string to_json(const FPCPResp& resp) {
	pt::ptree root;
	root.put("reqId", resp.reqId);
	root.put("error", resp.error);
	if (resp.scene) {
		pt::ptree scene_json;
		to_json(*resp.scene, scene_json);
		root.add_child("scene", scene_json);
	}

	std::ostringstream out;
	pt::write_json(out, root);
	return out.str();
}

void read_json(string json, pt::ptree& root) {
	std::stringstream ss;
	ss << json;
	pt::read_json(ss, root);
}

void from_json(string json, FPCPReq& req) {
	pt::ptree root;
	read_json(json, root);
}

string to_json(const fproc::Scene& scene) {
	pt::ptree root;
	to_json(scene, root);
	std::ostringstream out;
	pt::write_json(out, root);
	return out.str();
}

void to_json(const fproc::Scene& scene, pt::ptree &root) {
	/*
	 {
		 "timestamp": 123412341234
		 "persons": [{Person1 JSON}, ...]
	 }
	 */
	// Create a root
	fproc::Timestamp ts = scene.since();
	root.put("timestamp", ts > 0 ? ts : -1);
	pt::ptree persons;
	to_json(scene.getFaces(), persons);
	root.add_child("persons", persons);
}

void to_json(const fproc::PFList &faces, pt::ptree &root) {
	/*
	 [ Person1, Person2 ...]
	 */
	for (auto &face : faces) {
		pt::ptree person_node;
		to_json(face, person_node);
		root.push_back(std::make_pair("", person_node));
	}
}

void to_json(const fproc::PFace face, pt::ptree &root) {
	/*
	 {
		 "id": "p1234",
		 "firstSeenAt": 1234621834612
		 "lostAt" 12348888888133
		 "faces": [ {FrameRegion1 JSON}, ... ]
	 }
	 */
	root.put("id", face->getId());
	root.put("firstSeenAt", face->firstTimeCatched());
	root.put("lostAt", face->lostTime());
	pt::ptree pictures_node;
	to_json(face->getImages(), pictures_node);
	root.add_child("faces", pictures_node);
}

void to_json(const fproc::FRList &frame_regions, pt::ptree &root) {
	for (auto &fregion : frame_regions) {
		pt::ptree picture_node;
		to_json(fregion, picture_node);
		root.push_back(std::make_pair("", picture_node));
	}
}

void to_json(const fproc::FrameRegion &fregion, pt::ptree &root) {
	root.put("imgId", fregion.getFrame()->getId());
	pt::ptree region_node;
	to_json(fregion.getRectangle(), region_node);
	root.add_child("region", region_node);
}

void to_json(const fproc::Rectangle &r, pt::ptree &root) {
	// {"l": 123, "t": 123, "r": 1234, "b": 1234}
	root.put("l", r.left());
	root.put("t", r.top());
	root.put("r", r.right());
	root.put("b", r.bottom());
}

}

