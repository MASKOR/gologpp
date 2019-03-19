#ifndef GOLOGPP_GRAMMAR_H_
#define GOLOGPP_GRAMMAR_H_

#include "field_access.h"
#include "utilities.h"
#include "statements.h"
#include "fluent.h"
#include "action.h"
#include "functions.h"
#include "domain.h"
#include "types.h"

#include <boost/spirit/include/qi_kleene.hpp>
#include <boost/spirit/include/qi_omit.hpp>
#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_expect.hpp>
#include <boost/spirit/include/qi_eoi.hpp>
#include <boost/spirit/home/qi/nonterminal/error_handler.hpp>

#include <boost/phoenix/bind/bind_function.hpp>

namespace gologpp {
namespace parser {


struct ProgramParser : grammar<VoidExpression *(Scope &)> {
	ProgramParser()
	: ProgramParser::base_type(program)
	{
		program = *( omit[ // Discard attributes, they just register themselves as Globals
			abstract_fluent()(_r1)
			| action(_r1)
			| exog_action(_r1)
			| function(_r1)
			| numeric_domain_decl(_r1)
			| symbolic_domain_decl(_r1)
			| string_domain_decl(_r1)
			| type_definition(_r1)
		] ) > statement(_r1) > eoi;

		on_error<rethrow>(program,
			phoenix::bind(&handle_error, _1, _3, _2, _4)
		);

		GOLOGPP_DEBUG_NODE(program);
	}

	rule<VoidExpression *(Scope &)> program;
	ActionParser<Action> action;
	ActionParser<ExogAction> exog_action;
	AbstractFunctionParser function;
	StatementParser statement;
	DomainDeclarationParser<NumericExpression> numeric_domain_decl;
	DomainDeclarationParser<SymbolicExpression> symbolic_domain_decl;
	DomainDeclarationParser<StringExpression> string_domain_decl;
	CompoundTypeParser type_definition;
};



} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_GRAMMAR_H_
