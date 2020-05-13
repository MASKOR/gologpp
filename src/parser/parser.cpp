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

#include <fstream>
#include <chrono>
#include <iostream>

#include <boost/spirit/include/qi_action.hpp>
#include <boost/spirit/include/qi_expect.hpp>
#include <boost/phoenix/object/dynamic_cast.hpp>
#include <boost/phoenix/operator/self.hpp>
#include <boost/phoenix/core/reference.hpp>

#include "statements.h"
#include "utilities.h"
#include "grammar.h"
#include "parser.h"

#include <model/error.h>


namespace gologpp {
namespace parser {



unique_ptr<Instruction> parse_string(rule<Instruction *(Scope &)> &parser, const std::string &code)
{
	Instruction *rv = nullptr;

	boost::spirit::qi::phrase_parse(
		iterator(code.cbegin()),
		iterator(code.cend()),
		parser(boost::phoenix::ref(global_scope())),
		gologpp_skipper(),
		rv
	);
	return unique_ptr<Instruction>(rv);
}


unique_ptr<Instruction> parse_recursive(rule<Instruction *(Scope &)> &parser, const std::string &filename)
{
	std::ifstream file(filename);
	if (!file.is_open())
		throw std::runtime_error(filename + ": " + ::strerror(errno));
	std::stringstream buffer;
	buffer << file.rdbuf();
	if (!file.good())
		throw std::runtime_error(filename + ": " + ::strerror(errno));

	std::string content { buffer.str() };
	std::string::size_type offs = 0;
	const string include = "#include";
	while((offs = content.find(include, offs)) != std::string::npos) {
		offs += include.length();
		offs = content.find_first_not_of(" \t\r\n", offs);
		if (content[offs] != '"')
			throw SyntaxError(content, offs, "Expected: \"");
		++offs;
		string cur_path = filename.substr(0, filename.find_last_of("/"));
		string inc_filename = content.substr(offs, content.find_first_of("\"", offs) - offs);
		BatParser any_definition;
		rule<Instruction *(Scope &)> r_include { any_definition(_r1) > eoi };
		on_error<rethrow>(r_include, phoenix::bind(&handle_error, _1, _3, _2, _4));
		parse_recursive(r_include, cur_path + '/' + inc_filename);
	}
	std::cout << "Parsing " << filename << "..." << std::endl;

	auto t1 = std::chrono::high_resolution_clock::now();

	unique_ptr<Instruction> rv = parse_string(parser, content);
	std::chrono::duration<double> td = std::chrono::high_resolution_clock::now() - t1;

	std::cout << "... done. Parsing took " << td.count() << " s." << std::endl;

	return rv;
}


unique_ptr<Instruction> parse_file(const std::string &filename) {
	BatParser any_definition;
	StatementParser statement;
	rule<Instruction *(Scope &)> r_main {
		omit [any_definition(_r1)]
		> statement(_r1)[ _val = _1 ]
		> eoi
	};

	on_error<rethrow>(r_main,
		phoenix::bind(&handle_error, _1, _3, _2, _4)
	);

	return parse_recursive(r_main, filename);
}

}
}
