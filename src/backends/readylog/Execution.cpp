#include "Execution.h"

#include <eclipseclass.h>
#include "Action.h"
#include <gologpp/Action.h>

#include <iostream>

namespace gologpp {
namespace readylog {

ExecutionContext::ExecutionContext()
: generic::ExecutionContext<readylog_impl_config_t>()
{
	ec_set_option_ptr(EC_OPTION_ECLIPSEDIR, (void *)ECLIPSE_DIR);
	std::cout << ECLIPSE_DIR;
	if (ec_init())
		throw std::runtime_error("Error initializing ECLiPSe subsystem");

	post_goal((string("consult(\"") + READYLOG_PATH + "\")").c_str());
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