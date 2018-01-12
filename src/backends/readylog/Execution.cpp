#include "Execution.h"

#include <eclipseclass.h>

namespace gologpp {
namespace readylog {

ExecutionContext::ExecutionContext()
: gologpp::ExecutionContext()
{
	if (!ec_init())
		throw std::runtime_error("Error initializing ECLiPSe subsystem");

	post_goal((string("consult(") + READYLOG_PATH + ")").c_str());
	EC_resume();
}


ExecutionContext::~ExecutionContext()
{
	ec_cleanup();
}


ExecutionContext &ExecutionContext::instance()
{
	static ExecutionContext ctx;
	return ctx;
}





}
}