#ifndef GOLOGPP_GRAMMAR_H_
#define GOLOGPP_GRAMMAR_H_

#include "utilities.h"
#include "statements.h"
#include "fluent.h"
#include "action.h"
#include "functions.h"
#include "domain.h"
#include "types.h"
#include "expressions.h"
#include "value.h"

#include <parser/platform/component.h>
#include <parser/platform/constraint.h>

#include <boost/spirit/include/qi_plus.hpp>
#include <boost/spirit/include/qi_omit.hpp>
#include <boost/spirit/include/qi_alternative.hpp>
#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_expect.hpp>
#include <boost/spirit/include/qi_eoi.hpp>
#include <boost/spirit/home/qi/nonterminal/error_handler.hpp>

#include <boost/phoenix/bind/bind_function.hpp>

namespace gologpp {
namespace parser {


struct BatParser : grammar<void(Scope &)> {
	BatParser()
	: BatParser::base_type(program)
	{
		program = expect[+(
			fluent(_r1)
			| action(_r1)
			| exog_action(_r1)
			| function(_r1)
			| exog_function(_r1)
			| procedure(_r1)
			| domain_decl()(_r1)
			| type_definition(_r1)
			| component(_r1)
			| constraint_section(_r1)
		) > eoi];

		// The rules that parse all the different expression types have to be defined
		// after all of the other high-level grammars have been initialized to break
		// the dependency cyle. Fortunately, we can reference a rule before it is defined.
		initialize_cyclic_expressions();
		initialize_cyclic_values();

		on_error<rethrow>(program, phoenix::bind(&throw_syntax_error, _1, _3, _2, _4));

		GOLOGPP_DEBUG_NODE(program)
	}

	rule<void(Scope &)> program;
	ComponentParser component;
	ConstraintSectionParser constraint_section;
	ActionParser<Action> action;
	ActionParser<ExogAction> exog_action;
	FunctionParser function;
	ExogFunctionParser exog_function;
	ProcedureParser procedure;
	TypeDefinitionParser type_definition;
	FluentParser fluent;
};



} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_GRAMMAR_H_
