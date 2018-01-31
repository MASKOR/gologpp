#include "Execution.h"

#include <eclipseclass.h>
#include "Action.h"
#include <gologpp/Action.h>

namespace gologpp {
namespace readylog {

ExecutionContext::ExecutionContext()
: generic::ExecutionContext()
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


void ExecutionContext::compile(const EC_word &term)
{
	post_goal(::term(EC_functor("compile_term", 1), term));
	int rv = ec_resume();
	if (rv != EC_status::EC_succeed)
		throw EclipseError();
}



}
}