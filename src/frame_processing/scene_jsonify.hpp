#ifndef SRC_FRAME_PROCESSING_SCENE_JSONIFY_HPP_
#define SRC_FRAME_PROCESSING_SCENE_JSONIFY_HPP_

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "../model.hpp"

namespace fproc {

namespace pt = boost::property_tree;

class SceneJsonify
{
public:
  SceneJsonify();
  void operator()(const Scene &scene, pt::ptree *root) const;
  const std::string operator()(const Scene &scene, bool pretty = true) const;
private:
  void toJson(const Scene &scene, pt::ptree *root) const;
  void toJson(const PFList &faces, pt::ptree *faces_node) const;
  void toJson(const FRList &pictures, pt::ptree *pictures_node) const;
  void toJson(const Rectangle &r, pt::ptree *out) const;
};

}
#endif // SRC_FRAME_PROCESSING_SCENE_JSONIFY_HPP_
