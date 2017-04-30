#ifndef SRC_FRAME_PROCESSING_UUID_HPP_
#define SRC_FRAME_PROCESSING_UUID_HPP_

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

namespace fproc {
class Uuid
{
public:
  Uuid();
  const std::string get();
private:
  boost::uuids::random_generator _uuid_generator;
};
}
#endif // SRC_FRAME_PROCESSING_UUID_HPP_
