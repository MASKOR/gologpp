#include <fstream>

#include "grammar.h"
#include "parser.h"

namespace gologpp {
namespace parser {


Scope &parse_string(const std::string &code)
{
	Scope *rv;
	ProgramParser program_parser;
	boost::spirit::qi::phrase_parse(
		code.cbegin(),
		code.cend(),
		program_parser,
		boost::spirit::ascii::space_type(),
		rv
	);
	return *rv;
}


Scope &parse_file(const std::string &filename)
{
	std::ifstream file(filename);
	std::stringstream buffer;
	buffer << file.rdbuf();
	return parse_string(buffer.str());
}

}
}