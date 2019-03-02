#ifndef GOLOGPP_PARSER_ACTION_H_
#define GOLOGPP_PARSER_ACTION_H_

#include "utilities.h"
#include "formula.h"
#include "domain.h"
#include "string_expression.h"
#include "symbolic_expression.h"
#include "compound_expression.h"
#include "effect_axiom.h"
#include "reference.h"


#include <model/scope.h>

namespace gologpp {
namespace parser {


template<class ActionT>
struct ActionDefinitionParser;


template<>
struct ActionDefinitionParser<Action>
: grammar < void (
	Scope &, // parent scope
	Scope *, // owned scope
	string, // action name
	boost::optional<vector<shared_ptr<AbstractVariable>>> // action args
) > {
	ActionDefinitionParser();

	rule < void (
		Scope &, // parent scope
		Scope *, // owned scope
		string, // action name
		boost::optional<vector<shared_ptr<AbstractVariable>>> // action args
	) > definition;

	ExpressionParser<BooleanExpression> formula_parser;
	rule<AbstractEffectAxiom *(Scope &)> effect;
	EffectParser<BooleanExpression> boolean_effect;
	EffectParser<NumericExpression> numeric_effect;
	EffectParser<SymbolicExpression> symbolic_effect;
	EffectParser<StringExpression> string_effect;
	EffectParser<CompoundExpression> compound_effect;
	AnyDomainAssignmentParser domain_assignment;

	ReferenceParser<BooleanFluent>boolean_senses;
	ReferenceParser<NumericFluent> numeric_senses;
	ReferenceParser<SymbolicFluent> symbolic_senses;
	ReferenceParser<StringFluent> string_senses;
	ReferenceParser<CompoundFluent> compound_senses;
	rule<Reference<AbstractFluent> *(Scope &)> senses;
};



template<>
struct ActionDefinitionParser<ExogAction>
: grammar < void (
	Scope &, // parent scope
	Scope *, // owned scope
	string, // action name
	boost::optional<vector<shared_ptr<AbstractVariable>>> // action args
) > {
	ActionDefinitionParser();

	rule < void (
		Scope &, // parent scope
		Scope *, // owned scope
		string, // action name
		boost::optional<vector<shared_ptr<AbstractVariable>>> // action args
	) > definition;

	ExpressionParser<BooleanExpression> formula_parser;
	rule<AbstractEffectAxiom *(Scope &)> effect;
	EffectParser<BooleanExpression> boolean_effect;
	EffectParser<NumericExpression> numeric_effect;
	EffectParser<SymbolicExpression> symbolic_effect;
	EffectParser<StringExpression> string_effect;
	EffectParser<CompoundExpression> compound_effect;
	AnyDomainAssignmentParser domain_assignment;
};



template<class ActionT>
struct ActionParser
: grammar <
	ActionT *(Scope &),
	locals <
		Scope *,
		string,
		boost::optional < vector < shared_ptr < AbstractVariable > > >
	>
> {
	ActionParser();

	ActionDefinitionParser<ActionT> action_definition;

	rule < ActionT *(Scope &), locals <
		Scope *,
		string,
		boost::optional < vector < shared_ptr < AbstractVariable > > >
	> > action;
};


extern template
struct ActionParser<Action>;

extern template
struct ActionParser<ExogAction>;



} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_ACTION_H_

