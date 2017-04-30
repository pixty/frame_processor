/*
 * logger.h
 *
 *  Created on: Jan 16, 2017
 *      Author: dmitry
 */

#ifndef SRC_LOGGER_HPP_
#define SRC_LOGGER_HPP_

#include <iostream>

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
void log(std::string level, const LogData<List> &data, bool print = true) {
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

#define LOG_INFO(x) (log("INFO  ", LogData<None>() << x))
#define LOG_WARN(x) (log("WARN  ", LogData<None>() << x))
#define LOG_ERROR(x) (log("ERROR ", LogData<None>() << x))
#define LOG_DEBUG(x) (log("DEBUG ", LogData<None>() << x, debugEnabled))

#endif /* SRC_LOGGER_HPP_ */
