#ifndef SRC_SCENE_HPP_
#define SRC_SCENE_HPP_

#include <list>
#include <unordered_map>

#include <dlib/geometry/rectangle.h>

namespace fp {
    class TPosition{
        const long _frameId;
        const dlib::rectangle _position;
        const bool _isFace;
    public:
        TPosition(const long frameId, const dlib::rectangle& position, bool isFace):
            _frameId(frameId),
            _position(position),
            _isFace(isFace) // ???
        {}

        bool isFace(){return _isFace;}
    };

    template <typename _Id>
    class Tracklet{
        std::list<TPosition> _sequence;
        const _Id _id;
    public:
        Tracklet(const _Id id, const TPosition &position)
            :_id(id)
        {
            _sequence.push_back(position);
        }

        const _Id& getId(){
            return _id;
        }

        void add(const TPosition &position)
        {
            _sequence.push_back(position);
        }

        const std::list<TPosition>::const_iterator getTrack() const
        {
            return _sequence.cbegin();
        }
    };

    template <typename _Id>
    class Scene
    {
        const std::unordered_map<_Id, Tracklet<_Id>> _tracklets;

    public:

        Scene();

        // returns a tracklet
        Tracklet<_Id> getTracklet(const _Id id){
            std::map<int, int> m;
            return *_tracklets.find(id);
        }

        // adds a tracklet
        void add(const Tracklet<_Id> tracklet){
            _tracklets.insert(std::make_pair(tracklet.getId(), tracklet)); // check for duplicates
        }

        // removes a tracklet
        void remove(const _Id &id){
            _tracklets.erase(id);
        }

        // returns tracklets
        typename std::unordered_map<const _Id, Tracklet<_Id>>::const_iterator
        getTracklets()
        {
            return _tracklets.cbegin();
        }

    };
}
#endif // SRC_SCENE_HPP_
