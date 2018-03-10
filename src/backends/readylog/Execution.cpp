#include "Execution.h"

#include <eclipseclass.h>
#include "Action.h"
#include <gologpp/Action.h>

#include <iostream>

namespace gologpp {

EclipseContext::EclipseContext()
: ExecutionContext()
{
	ec_set_option_ptr(EC_OPTION_ECLIPSEDIR, (void *)ECLIPSE_DIR);
	std::cout << "Using eclipse-clp in " << ECLIPSE_DIR << std::endl;
	if (ec_init())
		throw std::runtime_error("Error initializing ECLiPSe subsystem");

	std::cout << "Loading readylog from " << READYLOG_PATH " ..." << std::endl;
	post_goal(::term(EC_functor("compile", 1), EC_atom(READYLOG_PATH)));
	EC_resume();
	std::cout << "... done." << std::endl;
}


EclipseContext::~EclipseContext()
{
	ec_cleanup();
}


EclipseContext &EclipseContext::instance()
{
	static EclipseContext ctx;
	return ctx;
}


void EclipseContext::compile(const EC_word &term)
{
	post_goal(::term(EC_functor("assert", 1), term));
	EC_word term_cp(term);
	EC_functor term_fn;
	term_cp.functor(&term_fn);
	post_goal(::term(EC_functor("listing", 1),
		EC_atom((string(term_fn.Name()) + "/" + std::to_string(term_fn.Arity())).c_str())
	));
	int rv = EC_resume();
	if (rv != EC_status::EC_succeed)
		throw EclipseError();
}



} // namespace gologpp
