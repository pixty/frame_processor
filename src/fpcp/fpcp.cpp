/*
 * fpcp.cpp
 *
 *  Created on: Mar 5, 2017
 *      Author: dmitry
 */
#include "fpcp.hpp"
#include "cpr/cpr.h"

namespace fpcp {

void FprocEndHttp::sendScene(const fproc::Scene& scene) {
	const char *content = "test";
	int length = 4;
	auto r = cpr::Post(cpr::Url{"http://www.httpbin.org/post"},
	                   cpr::Multipart{{"key", "large value"},
	                                  {"name", cpr::Buffer{content, content + length, "filename.txt"}}});

}

}


