 
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

#include <model/formula.h>
#include <model/logger.h>
#include <model/reference.h>
#include <model/action.h>
#include <model/effect_axiom.h>
#include <model/fluent.h>
#include <model/procedural.h>

#include <semantics/readylog/execution.h>
#include <semantics/readylog/procedural.h>

#include <parser/parser.h>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/spirit/home/qi/detail/expectation_failure.hpp>

#include <model/platform/semantics.h>





using namespace gologpp;
namespace po = boost::program_options;

int test_file(unique_ptr<Instruction> &&mainproc)
{
	shared_ptr<Function> f_postcond = global_scope().lookup_global<Function>("postcond");
	unique_ptr<Reference<Function>> postcond;
	if (f_postcond) {
		postcond.reset(f_postcond->make_ref({}));
		postcond->attach_semantics(ReadylogContext::instance().semantics_factory());
	}

	mainproc->attach_semantics(ReadylogContext::instance().semantics_factory());

	ReadylogContext::instance().run(*mainproc);

	if (postcond) {
		log(LogLevel::INF) << "Testing postcond(): " << flush;
		bool success = static_cast<bool>(
			postcond->semantics().evaluate({}, ReadylogContext::instance().history())
		);

		if (success) {
			log(LogLevel::INF) << "OK" << flush;
			return 0;
		}
		else {
			log(LogLevel::ERR) << "FAIL" << flush;
			return 1;
		}
	}
	else
		return 0;
}


int main(int argc, char **argv) {
	boost::filesystem::path binary_path(argv[0]);
	po::options_description desc("Usage: " + binary_path.filename().string() + " [options...] [-f|--file] FILENAME\n\nOptions");
	desc.add_options()
		("help,h", "This help message")
		("file,f", po::value<string>(), "golog++ test program to process")
		("trace,t", "Trace calls to eclipse-clp")
		("guitrace,g", "Wait for graphical eclipse-clp tracer to attach")
		("loglevel,l", po::value<int>(), "Set loglevel [0..4], default 3")
	;
	po::positional_options_description p;
	p.add("file", -1);
	po::variables_map vm;

	try {
		po::store(
			po::command_line_parser(argc, argv).options(desc).positional(p).run(),
			vm
		);

		string filename = vm["file"].as<string>();
		int loglevel = 3;

		if (vm.count("loglevel"))
			loglevel = std::max(0, std::min(4, vm["loglevel"].as<int>()));

		Logger::instance().log_lvl() = static_cast<LogLevel>(loglevel);

		try {
			parser::parse_file(filename);
			shared_ptr<Procedure> mainproc = global_scope().lookup_global<Procedure>("main");
			if (!mainproc) {
				log(LogLevel::ERR) << "No procedure main() in " << filename << flush;
				return -2;
			}

			eclipse_opts options;
			options.trace = !vm["trace"].empty() || !vm["guitrace"].empty();
			options.toplevel = false;
			options.guitrace = !vm["guitrace"].empty();

			ReadylogContext::init(options);

			int rv = test_file(unique_ptr<Instruction>(mainproc->ref({})));

			ReadylogContext::shutdown();

			return rv;
		} catch (SyntaxError &e) {
			log(LogLevel::ERR) << e.what() << flush;
			return -2;
		}

	} catch (boost::bad_any_cast &e) {
		desc.print(std::cerr);
		return -1;
	}
}
