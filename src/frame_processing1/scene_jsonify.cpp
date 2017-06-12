#include "../frame_processing1/scene_jsonify.hpp"

namespace fproc {

SceneJsonify::SceneJsonify(){
}


void SceneJsonify::operator()(const Scene &scene, pt::ptree *root) const{
    toJson(scene, root);
}

const std::string SceneJsonify::operator()(const Scene &scene, bool pretty) const {
  pt::ptree root;
  toJson(scene, &root);
  std::ostringstream out;
  pt::write_json(out, root, pretty);
  return out.str();
}

void SceneJsonify::toJson(const Scene &scene, pt::ptree *root) const{
  /*
  scene
  {
    "timestamp": 123412341234
    "persons": [{Person1 JSON}, ...]
  }
  */
  // Create a root
  root->put("timestamp", scene.since());
  pt::ptree faces_node;
  toJson(scene.getFaces(), &faces_node);  
  root->add_child("persons", faces_node);    
}

void SceneJsonify::toJson(const PFList &faces, pt::ptree *faces_node) const{
/*
Person
{
    "id": "p1234",
    "firstSeenAt": 1234621834612
    "lostAt" 12348888888133
    "faces": [ {FrameRegion1 JSON}, ... ]
}
*/
  for (auto &face : faces){
    // Create an unnamed node containing the value
    pt::ptree face_node;
    face_node.put("id", face->getId());
    face_node.put("firstSeenAt", face->getId());
    face_node.put("firstSeenAt", face->firstTimeCatched());
    face_node.put("lostAt", face->lostTime());
    // save picture
    pt::ptree pictures_node;
    toJson(face->getImages(), &pictures_node);
    face_node.add_child("faces", pictures_node);
    // Add this node to the list.
    faces_node->push_back(std::make_pair("", face_node));
  }
}

void SceneJsonify::toJson(const FRList &pictures, pt::ptree *pictures_node) const{
    //{"imgId": "pic1234", "region": {Rectangle JSON}}
    for (auto &picture : pictures){
	// Create an unnamed node containing the value
	pt::ptree picture_node;
	picture_node.put("imgId", picture.getFrame()->getId());
	pt::ptree region_node;
	toJson(picture.getRectangle(), &region_node);
	picture_node.add_child("region", region_node);
	pictures_node->push_back(std::make_pair("", picture_node));
    }
}

void SceneJsonify::toJson(const Rectangle &r, pt::ptree *out) const{
  // {"l": 123, "t": 123, "r": 1234, "b": 1234}
  out->put("l", r.left());
  out->put("t", r.top());
  out->put("r", r.right());
  out->put("b", r.bottom());
}

}
