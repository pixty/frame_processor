#ifndef SRC_SIMPLE_PROFILER_HPP_
#define SRC_SIMPLE_PROFILER_HPP_

#include <string>
#include <dlib/timing.h>

namespace fp {
    class SimpleProfiler{
    public:
        SimpleProfiler(const char *name):
            _name(name),
            _id(++LatestId)
        {
        }

        inline void enter(){
            dlib::timing::start(_id, _name.c_str());
        }

        inline void exit(){
            dlib::timing::stop(_id);
        }

        inline void print(){
            dlib::timing::print();
        }
    private:
        static int LatestId;
        const std::string _name;
        const int _id;
    };
}
#endif // SRC_SIMPLE_PROFILER_HPP_
