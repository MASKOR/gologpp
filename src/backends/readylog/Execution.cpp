#include "Execution.h"

#include <eclipseclass.h>
#include "Action.h"
#include <gologpp/Action.h>
#include "Procedure.h"
#include "Fluent.h"

#include <iostream>

namespace gologpp {


Implementation<History>::Implementation(History &history)
: HistoryImplementation(history)
, history_len_(0)
{
	readylog_history_ = ::list(EC_atom("s0"), ::nil());
}


void Implementation<History>::append_exog(ExogTransition &&trans)
{
	readylog_history_ = ::list(trans.implementation().term(), readylog_history_);
}


EC_ref Implementation<History>::current_history()
{ return readylog_history_; }


void Implementation<History>::set_current_history(EC_ref h)
{ readylog_history_ = h; }


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


void EclipseContext::compile(const Block &block)
{ compile(block.implementation().term()); }


void EclipseContext::compile(const AbstractAction &action)
{
	try {
		Implementation<Action> action_impl = action.implementation<Action>();
		compile(action_impl.prim_action());
		compile(action_impl.prolog_poss_decl());
		compile(action_impl.prolog_poss());
		for (EC_word &ssa : action_impl.SSAs())
			compile(ssa);
	} catch (std::bad_cast &) {
		Implementation<ExogAction> action_impl = action.implementation<ExogAction>();
		compile(action_impl.exog_action());
		for (EC_word &ssa : action_impl.SSAs())
			compile(ssa);
	}
}


void EclipseContext::compile(const AbstractFluent &fluent)
{ compile(fluent.implementation<AbstractFluent>().prim_fluent()); }


void EclipseContext::compile(const AbstractFunction &function)
{ compile(function.implementation<AbstractFunction>().definition()); }


void EclipseContext::compile(const EC_word &term)
{
	post_goal(::term(EC_functor("assert", 1), term));
	EC_word term_cp(term);
	EC_functor term_fn;
	term_cp.functor(&term_fn);
	post_goal(::term(EC_functor("listing", 1),
		::term(EC_functor("/", 2),
			EC_atom(term_fn.Name()),
			term_fn.Arity()
		)
	));
	int rv = EC_resume();
	if (rv != EC_status::EC_succeed)
		throw EclipseError();
}


bool EclipseContext::final(Block &program, History &history)
{
	post_goal(::term(EC_functor("final", 2),
		program.implementation().current_program(),
		history.implementation().current_history()
	));
	return EC_resume() == EC_status::EC_succeed;
}


bool EclipseContext::trans(Block &e, History &h)
{
	EC_ref h1, e1;
	post_goal(::term(EC_functor("trans", 4),
		e.implementation().current_program(),
		h.implementation().current_history(),
		e1, h1
	));
	int rv = EC_resume();

	if (rv == EC_status::EC_succeed) {
		e.implementation().set_current_program(e1);
		h.implementation().set_current_history(h1);
		return true;
	}
	return false;
}



} // namespace gologpp
