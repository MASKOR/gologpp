 
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


using namespace gologpp;

int test_file(const string &filename)
{
	log(LogLevel::INF) << "Testing " << filename << "..." << flush;
	unique_ptr<Instruction> mainproc = parser::parse_file(filename);

	unique_ptr<Reference<Function>> postcond {
		global_scope().lookup_global<Function>("postcond")->make_ref({})
	};
	postcond->attach_semantics(ReadylogContext::instance().semantics_factory());

	if (!postcond) {
		log(LogLevel::ERR) << "No bool function postcond() in " << filename << flush;
		return -2;
	}

	ReadylogContext::instance().run(Block(
		new Scope(global_scope()),
		{ mainproc.release() }
	));

	Binding<Value> empty_binding;
	empty_binding.attach_semantics(ReadylogContext::instance().semantics_factory());

	bool success = static_cast<bool>(
		postcond->semantics().evaluate(empty_binding, ReadylogContext::instance().history())
	);

	if (success) {
		log(LogLevel::INF) << "... " << filename << " OK";
		return 0;
	}
	else {
		log(LogLevel::ERR) << "... " << filename << " FAIL";
		return 1;
	}
}


int main(int argc, char **argv) {
	string filename;
	if (argc != 2) {
		log(LogLevel::ERR) << "Usage: golog++test FILENAME" << flush;
		return -1;
	}
		filename = argv[1];

	eclipse_opts options;
	options.trace = true;
	options.toplevel = false;
	options.guitrace = true;

	ReadylogContext::init(options);

	int rv = test_file(filename);

	ReadylogContext::shutdown();

	return rv;
}
