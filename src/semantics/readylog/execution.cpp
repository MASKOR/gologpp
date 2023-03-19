/*************************************************************************
 * This file is part of golog++.
 *
 * golog++ is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * golog++ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with golog++.  If not, see <https://www.gnu.org/licenses/>.
**************************************************************************/

#include "wrap_eclipseclass.h"
#include <experimental/filesystem>

#include "value.h"
#include "execution.h"
#include "action.h"
#include "procedural.h"
#include "fluent.h"
#include "utilities.h"
#include "history.h"
#include "transition.h"

#include <execution/plan.h>
#include <execution/transformation.h>

#include <model/action.h>
#include <model/logger.h>

#include <model/platform/semantics.h>

namespace filesystem = std::experimental::filesystem;

namespace gologpp {


unique_ptr<ReadylogContext> ReadylogContext::instance_;


void ReadylogContext::shutdown()
{
	instance_->terminate();
	instance_.reset();
	ec_cleanup();
}


ReadylogContext::ReadylogContext(
	const eclipse_opts &options,
	unique_ptr<PlatformBackend> &&exec_backend,
	unique_ptr<PlanTransformation> &&transformation
)
: ExecutionController(
  	std::move(exec_backend),
  	std::move(transformation)
  )
, options_(options)
{
	Logger::Guard lg(LogLevel::DBG);
	ec_set_option_ptr(EC_OPTION_ECLIPSEDIR, const_cast<void *>(static_cast<const void *>(ECLIPSE_DIR)));
	std::cout << "Using eclipse-clp in " << ECLIPSE_DIR << std::endl;

	int rv;
	if ((rv = ec_init()))
		throw std::runtime_error("Error " + std::to_string(rv) + " initializing ECLiPSe subsystem");

	ec_start_ = new EC_ref();

	std::string readylog_path = find_readylog();
	std::cout << "Loading readylog from " << readylog_path << " ..." << std::endl;

	if (options.trace)
		post_goal("set_flag(debug_compile, on)");
	else
		post_goal("set_flag(debug_compile, off)");

	post_goal(::term(EC_functor("compile", 1), EC_atom(readylog_path.c_str())));

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
	
	string boilerplate_path = find_boilerplate();
	log(LogLevel::INF) << "Loading " << boilerplate_path << "..." << flush;
	ec_query(::term(EC_functor("compile", 1),
		EC_atom(boilerplate_path.c_str())
	));
	log(LogLevel::INF) << "... done." << flush;

	// Boilerplate stuff
	compile_term(::term(EC_functor("events_list", 1), ::nil()));
	compile_term(::term(EC_functor("param_cycletime", 1), EC_word(99999999)));

}


ReadylogContext::~ReadylogContext()
{}

void ReadylogContext::init(const eclipse_opts &options, unique_ptr<PlatformBackend> &&backend, unique_ptr<PlanTransformation> &&transformation)
{
	instance_ = unique_ptr<ReadylogContext>(new ReadylogContext(
		options,
		std::move(backend),
		std::move(transformation)
	) );
}


ReadylogContext &ReadylogContext::instance()
{ return *instance_; }

void ReadylogContext::precompile()
{}


void ReadylogContext::compile(const Action &action)
{
	Semantics<Action> &action_impl = action.special_semantics();
	compile_term(action_impl.durative_action());
	compile_term(action_impl.durative_poss());
	for (EC_word causes_val : action_impl.durative_causes_vals())
		compile_term(causes_val);
	if (action.senses())
		compile_term(action_impl.senses());
		// senses/2 declaration not really needed since golog++ appends
		// the sensing result to the history, anyways
}

void ReadylogContext::compile(const ExogAction &action) {
	Semantics<ExogAction> &action_impl = action.special_semantics();
	compile_term(action_impl.exog_action());
	compile_term(action_impl.poss());
	for (EC_word &causes_val : action_impl.causes_vals())
		compile_term(causes_val);
}


void ReadylogContext::compile(const Fluent &fluent)
{
	compile_term(fluent.special_semantics().prim_fluent());
	for (EC_word &initially : fluent.special_semantics().initially())
		compile_term(initially);
}


void ReadylogContext::compile(const Function &function)
{ compile_term(function.special_semantics().definition()); }

void ReadylogContext::compile(const ExogFunction &ef)
{ compile_term(ef.special_semantics().definition()); }

void ReadylogContext::compile(const Procedure &proc)
{ compile_term(proc.special_semantics().definition()); }


void ReadylogContext::compile_term(const EC_word &term)
{
	if (! ec_query (
	::term(EC_functor("compile_term", 1), term)
	) )
		throw EclipseError("Failed to compile_term/1");
	ec_write(term);
}

std::string ReadylogContext::find_readylog() {
	const char *readylog_pl = std::getenv("READYLOG_PL");
	std::string readylog_path_env = "";
	if (readylog_pl) {
		readylog_path_env = std::string(readylog_pl) + ":";
	}
	readylog_path_env += (std::string(SEMANTICS_INSTALL_DIR) + "/readylog/interpreter");
	std::size_t last = 0;
	std::size_t next;
	while (true) {
		next = readylog_path_env.find(':', last);
		std::string next_path = readylog_path_env.substr(last, next - last);
		if (next_path != "") {
			filesystem::path readylog_path(next_path);
			readylog_path /= "preprocessor.pl";
			if (filesystem::exists(readylog_path))
				return std::string(readylog_path);
		}
		if (next == std::string::npos) {
			throw std::runtime_error("Could not find ReadyLog in \"" + readylog_path_env
			                         + "\" please set READYLOG_PL to the ReadyLog path!");
		}
		last = next + 1;
	}
}

std::string ReadylogContext::find_boilerplate() {
	filesystem::path boilerplate_src_path{SOURCE_DIR};
	boilerplate_src_path /= "src/semantics/readylog/boilerplate.pl";
	if (filesystem::exists(boilerplate_src_path)) {
		return boilerplate_src_path.string();
	}
	filesystem::path boilerplate_install_path{SEMANTICS_INSTALL_DIR};
	boilerplate_install_path /= "readylog/boilerplate.pl";
	if (filesystem::exists(boilerplate_install_path)) {
		return boilerplate_install_path.string();
	}
	throw std::runtime_error("Could not find readylog boilerplate in " + boilerplate_src_path.string()
	                         + " or " + boilerplate_install_path.string());
}


void ReadylogContext::mark_vars_dead()
{
	for (Semantics<Variable> *vs : Semantics<Variable>::all_vars)
		vs->mark_dead();
}


void ReadylogContext::postcompile()
{
	Logger::Guard lg(LogLevel::DBG);
	if (!ec_query(EC_atom("compile_SSAs")))
		throw Bug("Failed to compile SSAs");
}


void ReadylogContext::ec_write(EC_word t)
{ ec_query(::term(EC_functor("printf", 2), "%Qw%n", ::list(t, ::nil()))); }


string ReadylogContext::to_string(EC_word t)
{
	EC_word S = ::newvar();
	EC_word q = ::term(EC_functor("sprintf", 3),
		S, "%Qw", ::list(t, ::nil())
	);
	char *rv;
	if(ec_query(q) && S.is_string(&rv) == EC_succeed)
		return rv;
	else
		throw Bug("Error converting eclipse term to string");
}

const eclipse_opts &ReadylogContext::options() const
{ return options_; }


void ReadylogContext::ec_cut()
{
	EC_resume();
	if (last_rv_ == EC_status::EC_succeed)
		ec_start_->cut_to();
}


bool ReadylogContext::ec_query(EC_word t)
{
	/* Insanity ensues:
	   Since the EC_ref::cut_to() mechanism seems to be broken, as well as post_goal("!"),
	   which simply has no effect, once/1 seems to be the ONLY way to cut choicepoints from
	   posted goals.

	   More info:
	   http://eclipseclp.org/doc/embedding/embroot081.html
	   https://sourceforge.net/p/eclipse-clp/mailman/message/23203000/
	   http://eclipseclp.org/archive/eclipse-users/0704.html
	*/
	//post_goal(::term(EC_functor("writeln", 1), t));
	post_goal(::term(EC_functor("once", 1), t));
	//post_goal(::term(EC_functor("writeln", 1), t));
	last_rv_ = EC_resume(*ec_start_);
	//std::cout << std::endl;

	mark_vars_dead();

	return last_rv_ == EC_status::EC_succeed;
}



} // namespace gologpp
