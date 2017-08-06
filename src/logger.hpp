/*
 * logger.h
 *
 *  Created on: Jan 16, 2017
 *      Author: dmitry
 */

#ifndef SRC_LOGGER_HPP_
#define SRC_LOGGER_HPP_

#include <iostream>
#include <map>

template <typename T1, typename T2>
inline std::ostream& operator<<(std::ostream& ostr, const std::multimap<T1, T2>& mm) {
	ostr << "[";
	for (typename std::multimap<T1, T2>::const_iterator it = mm.begin(); it != mm.end();++it) {
		ostr << (*it).first << " " << (*it).second;
	};
	ostr << "]";
	return ostr;
}

struct None {
};

template<typename First, typename Second>
struct Pair {
	First first;
	Second second;
};

template<typename List>
struct LogData {
	List list;
};

template<typename Begin, typename Value>
LogData<Pair<Begin, const Value &>> operator<<(LogData<Begin> begin,
		const Value &value) {
	return { {begin.list,value}};
}

template<typename Begin, size_t n>
LogData<Pair<Begin, const char *>> operator<<(LogData<Begin> begin,
		const char (&value)[n]) {
	return { {begin.list,value}};
}

inline void printList(std::ostream &os, None) {
}

template<typename Begin, typename Last>
void printList(std::ostream &os, const Pair<Begin, Last> &data) {
	printList(os, data.first);
	os << data.second;
}

void print_now();

template<typename List>
void ____log____(std::string level, const LogData<List> &data, bool print = true) {
	if (!print) {
		return;
	}
	print_now();
	std::cout << " " << level << ": ";
	printList(std::cout, data.list);
	std::cout << std::endl;
}

extern bool debugEnabled;

inline void debug_enabled(bool debugEnabled) {
	::debugEnabled = debugEnabled;
}

#define LOG_INFO(x) (____log____("INFO  ", LogData<None>() << x))
#define LOG_WARN(x) (____log____("WARN  ", LogData<None>() << x))
#define LOG_ERROR(x) (____log____("ERROR ", LogData<None>() << x))
#define LOG_DEBUG(x) (____log____("DEBUG ", LogData<None>() << x, debugEnabled))

#endif /* SRC_LOGGER_HPP_ */
