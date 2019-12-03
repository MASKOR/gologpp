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

#include <model/action.h>
#include <model/effect_axiom.h>
#include <model/fluent.h>
#include <model/procedural.h>
#include <semantics/readylog/execution.h>
#include <parser/parser.h>


using namespace gologpp;


void test_parser(const string &filename)
{
	Instruction *mainproc = parser::parse_file(filename).release();

	for (shared_ptr<const Global> g : global_scope().globals())
		std::cout << g->str() << std::endl;

	std::cout << mainproc->str() << std::endl;

	eclipse_opts options;
	options.trace = false;
	options.toplevel = false;
	options.guitrace = false;

	ReadylogContext::init(options);
	ReadylogContext &ctx = ReadylogContext::instance();

	ctx.run(Block(
		new Scope(global_scope()),
		{ mainproc }
	));

	ReadylogContext::shutdown();
}



int main(int argc, const char **argv)
{
	string filename = SOURCE_DIR "/examples/blocksworld.gpp";
	if (argc > 1)
		filename = argv[1];
	test_parser(filename);

	return 0;
}





