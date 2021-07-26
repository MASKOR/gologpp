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

#include <memory>
#include <iostream>
#include <atomic>

#include <model/formula.h>
#include <model/reference.h>
#include <model/logger.h>

#include <model/action.h>
#include <model/effect_axiom.h>
#include <model/fluent.h>
#include <model/procedural.h>
#include <semantics/native/execution.h>
#include <parser/parser.h>


using namespace gologpp;


int test_file(unique_ptr<Instruction> &&mainproc)
{
	shared_ptr<Function> f_postcond = global_scope().lookup_global<Function>("postcond");
	unique_ptr<Reference<Function>> postcond;
	if (f_postcond) {
		postcond.reset(f_postcond->make_ref({}));
		postcond->attach_semantics(NativeContext::instance().semantics_factory());
	}

	mainproc->attach_semantics(NativeContext::instance().semantics_factory());

	NativeContext::instance().run(*mainproc);

	if (postcond) {
		log(LogLevel::INF) << "Testing postcond(): " << flush;
		bool success = static_cast<bool>(
			postcond->semantics().evaluate({}, NativeContext::instance().history())
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


int main(int argc, const char **argv)
{
	string filename = SOURCE_DIR "/examples/blocksworld.gpp";
	if (argc > 1)
		filename = argv[1];

	parser::parse_file(filename);
	shared_ptr<Procedure> mainproc = global_scope().lookup_global<Procedure>("main");
	if (!mainproc) {
		log(LogLevel::ERR) << "No procedure main() in " << filename << flush;
		return -2;
	}

	int rv = test_file(unique_ptr<Instruction>(mainproc->ref({})));

	return rv;
}





