#include "execution.h"

#include <eclipseclass.h>
#include "action.h"
#include <model/action.h>
#include "procedural.h"
#include "fluent.h"
#include "utilities.h"

#include <iostream>

namespace gologpp {


Semantics<History>::Semantics(History &history)
: HistorySemantics(history)
{
	readylog_history_ = ::list(EC_atom("s0"), ::nil());
}


void Semantics<History>::append_exog(ExogTransition &&trans)
{
	readylog_history_ = ::list(trans.semantics().plterm(), readylog_history_);
}


EC_word Semantics<History>::current_history()
{
	return copy_term(readylog_history_);
}


void Semantics<History>::set_current_history(EC_word h)
{
	readylog_history_ = copy_term(h);
}


EC_word Semantics<History>::get_list_head(EC_word list)
{
    EC_word head, tail;
    if (list.is_list(head, tail))
        std::cout<<EC_fail<<std::endl;
    return head;
}


char* Semantics<History>::get_head_name(EC_word head)
{
    EC_functor headfunctor;
    head.functor(&headfunctor);
    return headfunctor.name();
}


Transition Semantics<History>::get_last_transition()
{
    EC_word head;
    char *headname;
    head = get_list_head(readylog_history_);
    headname = get_head_name(head);
    double d;
    long i;
    EC_atom did;
    char *s;
    EC_word term;
    vector<unique_ptr<AbstractConstant>> args;
    for(int j = 1; j <= head.arity();j++){
        head.arg(j,term);
        if (EC_succeed == term.is_double(&d)){
            args.emplace_back(new NumericConstant(d));
        }
        else if (EC_succeed == term.is_long(&i))
            std::cout << i << "\n";
        else if (EC_succeed == term.is_atom(&did))
            std::cout << did.Name() << "\n";
        else if (EC_succeed == term.is_string(&s))
            std::cout << s << "\n";
        else
            std::cout << "not a simple type\n";
    }
    shared_ptr<Action> on_shared = global_scope().lookup_global<Action>(headname, (arity_t)head.arity());
    Transition trans(on_shared,std::move(args));
    return trans;
}


unique_ptr<ReadylogContext> ReadylogContext::instance_;


void ReadylogContext::init(unique_ptr<AExecutionBackend> &&exec_backend, const eclipse_opts &options)
{ instance_ = unique_ptr<ReadylogContext>(new ReadylogContext(std::move(exec_backend), options)); }

void ReadylogContext::shutdown()
{ instance_.reset(); }


ReadylogContext::ReadylogContext(unique_ptr<AExecutionBackend> &&exec_backend, const eclipse_opts &options)
: ExecutionContext(std::make_unique<ReadylogSemanticsFactory>(), std::move(exec_backend))
, options_(options)
{
	ec_set_option_ptr(EC_OPTION_ECLIPSEDIR, const_cast<void *>(static_cast<const void *>(ECLIPSE_DIR)));
	std::cout << "Using eclipse-clp in " << ECLIPSE_DIR << std::endl;

	int rv;
	if ((rv = ec_init()))
		throw std::runtime_error("Error " + std::to_string(rv) + " initializing ECLiPSe subsystem");

	ec_start_ = new EC_ref();

	std::cout << "Loading readylog from " << READYLOG_PATH " ..." << std::endl;

	if (options.trace)
		post_goal("set_flag(debug_compile, on)");

	post_goal(::term(EC_functor("compile", 1), EC_atom(READYLOG_PATH)));

	if ((last_rv_ = EC_resume(*ec_start_)) == EC_status::EC_succeed)
		std::cout << "... done." << std::endl;
	else
		throw std::runtime_error("Error " + std::to_string(rv) + " loading readylog interpreter");

	if (options.trace) {
		std::cout << "Enabling ECLiPSe debugging." << std::endl;
		if (options.guitrace) {
			post_goal("lib(remote_tools)");
			post_goal("attach_tools");
		}
		else {
			post_goal("lib(tracer_tty)");
			post_goal("lib(toplevel)");
			post_goal("toplevel_init(tty)");
		}
	}

	//ec_query(EC_atom("toggle_dtdebug"));
	
	ec_query(::term(EC_functor("compile", 1),
		EC_atom(SOURCE_DIR "/src/semantics/readylog/boilerplate.pl")
	));
}


ReadylogContext::~ReadylogContext()
{
	ec_cleanup();
}


ReadylogContext &ReadylogContext::instance()
{ return *instance_; }


void ReadylogContext::compile(const Block &block)
{
	// Discard result since this is only called for the toplevel program,
	// which only needs to initialize its internal state.
	block.semantics().plterm();

	// Boilerplate stuff
	compile_term(::term(EC_functor("events_list", 1), ::nil()));
	compile_term(::term(EC_functor("param_cycletime", 1), EC_word(99999999)));
}


void ReadylogContext::compile(const AbstractAction &action)
{
	try {
		Semantics<Action> action_impl = action.semantics<Action>();
		compile_term(action_impl.prim_action());
		//compile_term(action_impl.prolog_poss_decl());
		//compile_term(action_impl.prolog_poss());
		compile_term(action_impl.poss());
		for (EC_word &ssa : action_impl.SSAs())
			compile_term(ssa);
	} catch (std::bad_cast &) {
		Semantics<ExogAction> action_impl = action.semantics<ExogAction>();
		compile_term(action_impl.exog_action());
		for (EC_word &ssa : action_impl.SSAs())
			compile_term(ssa);
	}
}


void ReadylogContext::compile(const AbstractFluent &fluent)
{
	compile_term(fluent.semantics<AbstractFluent>().prim_fluent());
	for (EC_word &initially : fluent.semantics<AbstractFluent>().initially())
		compile_term(initially);
}


void ReadylogContext::compile(const AbstractFunction &function)
{ compile_term(function.semantics<AbstractFunction>().definition()); }


void ReadylogContext::compile_term(const EC_word &term)
{
	if (! ec_query (
		::term(EC_functor("compile_term", 1), term)
	) )
		throw EclipseError();
	ec_write(term);
}


void ReadylogContext::ec_write(EC_word t)
{
	ec_query(::term(EC_functor("writeln", 1), t));
}


void ReadylogContext::ec_cut()
{
	EC_resume();
	if (last_rv_ == EC_status::EC_succeed)
		ec_start_->cut_to();
}


bool ReadylogContext::final(Block &program, History &history)
{
	EC_word final = ::term(EC_functor("final", 2),
		program.semantics().current_program(),
		history.semantics().current_history()
	);
	bool rv = ec_query(final);
	return rv;
}


bool ReadylogContext::trans(Block &program, History &history)
{
	EC_ref h1, e1;

	if (options_.trace && !options_.guitrace) {
		post_goal("toplevel");
		EC_resume();
	}//*/

	EC_word trans = ::term(EC_functor("trans", 4),
		program.semantics().current_program(),
		history.semantics().current_history(),
		e1, h1
	);

	EC_word q;
	if (options_.trace)
		q = ::term(EC_functor("trace", 1), trans);
	else
		q = trans;

	if (ec_query(q)) {
		program.semantics().set_current_program(e1);
		history.semantics().set_current_history(h1);
		return true;
	}
	return false;
}



} // namespace gologpp
