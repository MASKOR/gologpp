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
 * along with golog++.	If not, see <https://www.gnu.org/licenses/>.
 **************************************************************************/

#include <chrono>
#include <fstream>
#include <iostream>

#include <boost/phoenix/object/dynamic_cast.hpp>
#include <boost/phoenix/operator/self.hpp>
#include <boost/spirit/include/qi_action.hpp>
#include <boost/spirit/include/qi_expect.hpp>
#include <boost/spirit/include/qi_lexeme.hpp>

#include "grammar.h"
#include "platform_constraints.h"
#include "platform_model.h"
#include "platform_parser.h"
#include "statements.h"
#include "utilities.h"

#include <model/error.h>
#include <model/platform_model.h>

namespace gologpp {
namespace parser {

vector<shared_ptr<PlatformConstraint>>
parse_constraints(const std::string &filename) {
	::gologpp::parser::ConstraintParser platform;

	vector<shared_ptr<PlatformConstraint>> v;
	shared_ptr<PlatformConstraint> elem;
	std::ifstream file(filename);
	if (!file.is_open())
		throw std::runtime_error(filename + ": " + ::strerror(errno));
	std::stringstream buffer;
	buffer << file.rdbuf();
	if (!file.good())
		throw std::runtime_error(filename + ": " + ::strerror(errno));

	string content{buffer.str()};

	::gologpp::parser::gologpp_skipper skipper;
	// auto skipper = ::boost::spirit::ascii::space;
	if (::boost::spirit::qi::phrase_parse(parser::iterator(content.cbegin()),
																				parser::iterator(content.cend()),
																				platform,
																				skipper
																				// ::gologpp::parser::gologpp_skipper()
																				// ::boost::spirit::qi::ascii::space
																				,
																				v)) {
		std::cout << "parsed constraint successfully" << std::endl;
	} else {
		std::cout << "error parsing constraint" << std::endl;
	}
	return v;
}

unique_ptr<PlatformModel> parse_platform_model(const std::string &filename) {
	::gologpp::parser::PlatformParser platform;

	unique_ptr<PlatformModel> v;
	std::ifstream file(filename);
	if (!file.is_open())
		throw std::runtime_error(filename + ": " + ::strerror(errno));
	std::stringstream buffer;
	buffer << file.rdbuf();
	if (!file.good())
		throw std::runtime_error(filename + ": " + ::strerror(errno));

	string content{buffer.str()};

	::gologpp::parser::gologpp_skipper skipper;
	if (::boost::spirit::qi::phrase_parse(parser::iterator(content.cbegin()),
																				parser::iterator(content.cend()),
																				platform,
																				skipper,
																				v)) {
		std::cout << "parsed platform model successfully" << std::endl;
	} else {
		std::cout << "error parsing platform model" << std::endl;
	}
	return v;
}

} // namespace parser
} // namespace gologpp
