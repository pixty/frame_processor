#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "uuid.hpp"

namespace fproc {

  Uuid::Uuid()
{

}

const std::string Uuid::get(){  
  return boost::lexical_cast<std::string>(_uuid_generator());
}

}