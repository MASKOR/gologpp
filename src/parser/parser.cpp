#include <fstream>

#include "grammar.h"
#include "parser.h"

#include <boost/phoenix/core/reference.hpp>


namespace gologpp {
namespace parser {



unique_ptr<Expression> parse_string(const std::string &code)
{
	Expression *rv = nullptr;
	ProgramParser program_parser;
	boost::spirit::qi::phrase_parse(
		iterator(code.cbegin()),
		iterator(code.cend()),
		program_parser(boost::phoenix::ref(global_scope())),
		gologpp_skipper(),
		rv
	);
	return unique_ptr<Expression>(rv);
}


unique_ptr<Expression> parse_file(const std::string &filename)
{
	std::ifstream file(filename);
	if (!file.is_open())
		throw std::runtime_error(filename + ": " + ::strerror(errno));
	std::stringstream buffer;
	buffer << file.rdbuf();
	if (!file.good())
		throw std::runtime_error(filename + ": " + ::strerror(errno));
	return parse_string(buffer.str());
}


}
}
