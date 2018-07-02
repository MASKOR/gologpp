#include <fstream>

#include <boost/phoenix/core/reference.hpp>

#include "grammar.h"
#include "parser.h"

namespace gologpp {
namespace parser {


unique_ptr<Statement> parse_string(const std::string &code)
{
	Statement *rv = nullptr;
	ProgramParser program_parser;
	boost::spirit::qi::phrase_parse(
		code.cbegin(),
		code.cend(),
		program_parser(boost::phoenix::ref(global_scope())),
		boost::spirit::ascii::space_type(),
		rv
	);
	return unique_ptr<Statement>(rv);
}


unique_ptr<Statement> parse_file(const std::string &filename)
{
	std::ifstream file(filename);
	std::stringstream buffer;
	buffer << file.rdbuf();
	return parse_string(buffer.str());
}


}
}
