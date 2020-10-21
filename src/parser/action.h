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

#ifndef GOLOGPP_PARSER_ACTION_H_
#define GOLOGPP_PARSER_ACTION_H_

#include "utilities.h"
#include "domain.h"
#include "effect_axiom.h"
#include "reference.h"
#include "formula.h"
#include "mapping.h"
#include "assignment.h"

#include <model/scope.h>
#include <execution/clock.h>

namespace gologpp {
namespace parser {


template<class ActionT>
struct ActionDefinitionParser
: grammar < void (
	Scope &, // parent scope
	Scope *, // owned scope
	string, // action name
	boost::optional<vector<shared_ptr<Variable>>> // action args
), locals<ActionT *> > {
	ActionDefinitionParser();


	rule < void (
		Scope &, // parent scope
		Scope *, // owned scope
		string, // action name
		boost::optional<vector<shared_ptr<Variable>>> // action args
	), locals<ActionT *> > definition;

	rule<AbstractEffectAxiom *(Scope &)> effect;
	rule<Clock::DurationRange()> duration;
	EffectParser<Reference<Fluent>> fluent_effect;
	EffectParser<FieldAccess> field_effect;
	EffectParser<ListAccess> list_effect;
	rule<AbstractAssignment *(Scope &)> senses;
	AssignmentParser<Reference<Fluent>> senses_fluent;
	AssignmentParser<ListAccess> senses_list_element;
	AssignmentParser<FieldAccess> senses_compound_field;
	MappingParser mapping;
	BooleanExpressionParser boolean_expression;
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

