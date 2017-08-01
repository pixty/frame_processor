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
void to_json(const fproc::PFaceList &faces, pt::ptree &root);
void to_json(const fproc::Face& face, pt::ptree &root);
void to_json(const fproc::PFrameRegList &frame_regions, pt::ptree &root);
void to_json(const fproc::FrameRegion &fregion, pt::ptree &root);
void to_json(const fproc::Rectangle &r, pt::ptree &root);


// Create necessary escape sequences from illegal characters
template<class Ch>
std::basic_string<Ch> create_escapes(const std::basic_string<Ch> &s)
{
    std::basic_string<Ch> result;
    typename std::basic_string<Ch>::const_iterator b = s.begin();
    typename std::basic_string<Ch>::const_iterator e = s.end();
    while (b != e)
    {
        typedef typename boost::make_unsigned<Ch>::type UCh;
        UCh c(*b);
        // This assumes an ASCII superset. But so does everything in PTree.
        // We escape everything outside ASCII, because this code can't
        // handle high unicode characters.
        if (c == 0x20 || c == 0x21 || (c >= 0x23 && c <= 0x2E) ||
            (c >= 0x30 && c <= 0x5B) || (c >= 0x5D && c <= 0xFF))
            result += *b;
        else if (*b == Ch('\b')) result += Ch('\\'), result += Ch('b');
        else if (*b == Ch('\f')) result += Ch('\\'), result += Ch('f');
        else if (*b == Ch('\n')) result += Ch('\\'), result += Ch('n');
        else if (*b == Ch('\r')) result += Ch('\\'), result += Ch('r');
        else if (*b == Ch('\t')) result += Ch('\\'), result += Ch('t');
        else if (*b == Ch('/')) result += Ch('\\'), result += Ch('/');
        else if (*b == Ch('"'))  /*result += Ch('\\'),*/ result += Ch('"');
        else if (*b == Ch('\\')) result += Ch('\\'), result += Ch('\\');
        else
        {
            const char *hexdigits = "0123456789ABCDEF";
            unsigned long u = (std::min)(static_cast<unsigned long>(
                                             static_cast<UCh>(*b)),
                                         0xFFFFul);
            int d1 = u / 4096; u -= d1 * 4096;
            int d2 = u / 256; u -= d2 * 256;
            int d3 = u / 16; u -= d3 * 16;
            int d4 = u;
            result += Ch('\\'); result += Ch('u');
            result += Ch(hexdigits[d1]); result += Ch(hexdigits[d2]);
            result += Ch(hexdigits[d3]); result += Ch(hexdigits[d4]);
        }
        ++b;
    }
    return result;
}

template<class Ptree>
void write_json_helper(std::basic_ostream<typename Ptree::key_type::value_type> &stream,
                       const Ptree &pt,
                       int indent, bool pretty)
{

    typedef typename Ptree::key_type::value_type Ch;
    typedef typename std::basic_string<Ch> Str;

    // Value or object or array
    if (indent > 0 && pt.empty())
    {
        // Write value
        Str data = create_escapes(pt.template get_value<Str>());
        stream << data;

    }
    else if (indent > 0 && pt.count(Str()) == pt.size())
    {
        // Write array
        stream << Ch('[');
        if (pretty) stream << Ch('\n');
        typename Ptree::const_iterator it = pt.begin();
        for (; it != pt.end(); ++it)
        {
            if (pretty) stream << Str(4 * (indent + 1), Ch(' '));
            write_json_helper(stream, it->second, indent + 1, pretty);
            if (boost::next(it) != pt.end())
                stream << Ch(',');
            if (pretty) stream << Ch('\n');
        }
        if (pretty) stream << Str(4 * indent, Ch(' '));
        stream << Ch(']');

    }
    else
    {
        // Write object
        stream << Ch('{');
        if (pretty) stream << Ch('\n');
        typename Ptree::const_iterator it = pt.begin();
        for (; it != pt.end(); ++it)
        {
            if (pretty) stream << Str(4 * (indent + 1), Ch(' '));
            stream << Ch('"') << create_escapes(it->first) << Ch('"') << Ch(':');
            if (pretty) stream << Ch(' ');
            write_json_helper(stream, it->second, indent + 1, pretty);
            if (boost::next(it) != pt.end())
                stream << Ch(',');
            if (pretty) stream << Ch('\n');
        }
        if (pretty) stream << Str(4 * indent, Ch(' '));
        stream << Ch('}');
    }

}

// Verify if ptree does not contain information that cannot be written to json
template<class Ptree>
bool verify_json(const Ptree &pt, int depth)
{

    typedef typename Ptree::key_type::value_type Ch;
    typedef typename std::basic_string<Ch> Str;

    // Root ptree cannot have data
    if (depth == 0 && !pt.template get_value<Str>().empty())
        return false;

    // Ptree cannot have both children and data
    if (!pt.template get_value<Str>().empty() && !pt.empty())
        return false;

    // Check children
    typename Ptree::const_iterator it = pt.begin();
    for (; it != pt.end(); ++it)
        if (!verify_json(it->second, depth + 1))
            return false;

    // Success
    return true;

}

// Write ptree to json stream
template<class Ptree>
void write_json_internal(std::basic_ostream<typename Ptree::key_type::value_type> &stream,
                         const Ptree &pt)
{
    write_json_helper(stream, pt, 0, true);
    stream << std::endl;
}

template <typename T>
struct my_id_translator
{
    typedef T internal_type;
    typedef T external_type;

    boost::optional<T> get_value(const T &v) { return  v.substr(1, v.size() - 2) ; }
    boost::optional<T> put_value(const T &v) { return '"' + v +'"'; }
};

string to_json(const FPCPResp& resp) {
	pt::ptree root;
	root.put<id>("reqId", resp.reqId, my_id_translator<string>());
	root.put<int>("error", resp.error);
	if (resp.scene) {
		pt::ptree scene_json;
		to_json(*resp.scene, scene_json);
		root.add_child("scene", scene_json);
	}

	if (resp.image) {
		pt::ptree image_json;
		to_json(*resp.image, image_json);
		root.add_child("image", image_json);
	}

	if (resp.person) {
		pt::ptree person_json;
		to_json(*resp.person, person_json);
		root.add_child("person", person_json);
	}

	std::ostringstream out;
	//pt::write_json(out, root);
	write_json_internal(out, root);
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

	req.reqId = root.get<id>("reqId", nullId);
	req.scene = root.get<bool>("scene", false);
	req.imgId = root.get<id>("imgId", nullId);
	req.personId = root.get<id>("personId", nullId);
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
	 	 "imgId": "12341234324",
		 "timestamp": 123412341234,
		 "persons": [{Person1 JSON}, ...]
	 }
	 */
	// Create a root
	fproc::Timestamp ts = scene.since();
	if (scene.frame().get()) {
		root.put("imgId", std::to_string(scene.frame()->getFrame()->getId()), my_id_translator<string>());
	}
	root.put<fproc::Timestamp>("timestamp", ts > 0 ? ts : -1);
	pt::ptree persons;
	to_json(scene.getFaces(), persons);
	if (!persons.empty())
		root.add_child("persons", persons);
}

void to_json(const fproc::PFaceList &faces, pt::ptree &root) {
	/*
	 [ Person1, Person2 ...]
	 */
	for (auto &face : faces) {
		pt::ptree person_node;
		to_json(*face, person_node);
		root.push_back(std::make_pair("", person_node));
	}
}

void to_json(const fproc::Face& face, pt::ptree &root) {
	/*
	 {
		 "id": "p1234",
		 "firstSeenAt": 1234621834612
		 "lostAt" 12348888888133
		 "faces": [ {FrameRegion1 JSON}, ... ]
	 }
	 */
	root.put<id>("id", face.getId(), my_id_translator<string>());
	root.put<fproc::Timestamp>("firstSeenAt", face.firstTimeCatched());
	root.put<fproc::Timestamp>("lostAt", face.lostTime());
	pt::ptree pictures_node;
	to_json(face.getImages(), pictures_node);
	if (!pictures_node.empty())
		root.add_child("faces", pictures_node);
}

void to_json(const fproc::PFrameRegList &frame_regions, pt::ptree &root) {
	for (auto &fregion : frame_regions) {
		pt::ptree picture_node;
		to_json(fregion, picture_node);
		root.push_back(std::make_pair("", picture_node));
	}
}

void to_json(const fproc::FrameRegion &fregion, pt::ptree &root) {
	root.put("imgId", std::to_string(fregion.getFrame()->getId()), my_id_translator<string>());
	pt::ptree region_node;
	to_json(fregion.getRectangle(), region_node);
	root.add_child("region", region_node);
	root.put<fproc::Timestamp>("timestamp", fregion.getFrame()->getTimestamp());
}

void to_json(const fproc::Rectangle &r, pt::ptree &root) {
	// {"l": 123, "t": 123, "r": 1234, "b": 1234}
	root.put<int>("l", r.left());
	root.put<int>("t", r.top());
	root.put<int>("r", r.right());
	root.put<int>("b", r.bottom());
}

}

