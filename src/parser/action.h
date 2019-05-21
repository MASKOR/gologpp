#ifndef GOLOGPP_PARSER_ACTION_H_
#define GOLOGPP_PARSER_ACTION_H_

#include "utilities.h"
#include "domain.h"
#include "effect_axiom.h"
#include "reference.h"

#include <model/scope.h>

namespace gologpp {
namespace parser {


template<class ActionT>
struct ActionDefinitionParser
: grammar < void (
	Scope &, // parent scope
	Scope *, // owned scope
	string, // action name
	boost::optional<vector<shared_ptr<Variable>>> // action args
) > {
	ActionDefinitionParser();


	rule < void (
		Scope &, // parent scope
		Scope *, // owned scope
		string, // action name
		boost::optional<vector<shared_ptr<Variable>>> // action args
	) > definition;

	rule<AbstractEffectAxiom *(Scope &)> effect;
	EffectParser<Reference<Fluent>> fluent_effect;
	EffectParser<FieldAccess> field_effect;
	ReferenceParser<Fluent> senses;
};



template<class ActionT>
struct ActionParser
: grammar <
	ActionT *(Scope &),
	locals <
		Scope *,
		string,
		boost::optional < vector < shared_ptr < Variable > > >
	>
> {
	ActionParser();

	ActionDefinitionParser<ActionT> action_definition;

	rule < ActionT *(Scope &), locals <
		Scope *,
		string,
		boost::optional < vector < shared_ptr < Variable > > >
	> > action;
};


extern template
struct ActionParser<Action>;

extern template
struct ActionParser<ExogAction>;



} // namespace parser
} // namespace gologpp


#endif // GOLOGPP_PARSER_ACTION_H_

