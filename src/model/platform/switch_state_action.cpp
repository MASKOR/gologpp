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

#include <model/effect_axiom.h>
#include <model/logger.h>

#include <execution/plan.h>

#include "switch_state_action.h"
#include "component.h"



namespace gologpp {
namespace platform {

SwitchStateAction::SwitchStateAction(Scope *own_scope)
: AbstractAction(
	own_scope,
	get_type<UndefinedType>(),
	"switch_state",
	{
		own_scope->get_var(VarDefinitionMode::FORCE, get_type<StringType>(), "component"),
		own_scope->get_var(VarDefinitionMode::FORCE, get_type<StringType>(), "from_state"),
		own_scope->get_var(VarDefinitionMode::FORCE, get_type<StringType>(), "to_state"),
	}
  )
, param_component_(own_scope->lookup_var("component"))
, param_from_state_(own_scope->lookup_var("from_state"))
, param_to_state_(own_scope->lookup_var("to_state"))
{}

void SwitchStateAction::attach_semantics(gologpp::SemanticsFactory &f)
{ AbstractAction::attach_semantics(f); }

string SwitchStateAction::to_string(const string &pfx) const
{ return pfx + name() + '(' + concat_list(params(), ", ", "") + ')' + " {}"; }

Instruction *SwitchStateAction::ref(const vector<Expression *> &args)
{ return make_ref(args); }

gologpp::Reference<SwitchStateAction> *SwitchStateAction::make_ref(const vector<Expression *> &args)
{ return make_ref_<SwitchStateAction>(args); }

void SwitchStateAction::set_mapping(ActionMapping *)
{}

shared_ptr<const Variable> SwitchStateAction::param_component() const
{ return param_component_; }

shared_ptr<const Variable> SwitchStateAction::param_from_state() const
{ return param_from_state_; }

shared_ptr<const Variable> SwitchStateAction::param_to_state() const
{ return param_to_state_; }


} // namespace platform

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

GeneralSemantics<Reference<platform::SwitchStateAction>>::GeneralSemantics(
	const Reference<platform::SwitchStateAction> &elem,
	AExecutionContext &context
)
: element_(&elem)
, context_(context)
, final_(false)
{}

bool GeneralSemantics<Reference<platform::SwitchStateAction> >::final(const Binding &, const History &)
{ return final_; }


unique_ptr<Plan> GeneralSemantics<Reference<platform::SwitchStateAction> >::trans(const Binding &, History &)
{
	string component_name = static_cast<string>(
		dynamic_cast<const Value &>(element().arg_for_param(element()->param_component()))
	);
	string from_state = static_cast<string>(
		dynamic_cast<const Value &>(element().arg_for_param(element()->param_from_state()))
	);
	string to_state = static_cast<string>(
		dynamic_cast<const Value &>(element().arg_for_param(element()->param_to_state()))
	);

	shared_ptr<platform::Component> component = element().scope().lookup_global<platform::Component>(component_name);

	if (component->current_state() != from_state)
		log(LogLevel::ERR) << "Component \"" << component_name << "\" expected to be in state \"" << from_state
			<< "\", but it's in state \"" << component->current_state().name() << "\"" << flush;
	// TODO: What should really happen in a case like this?

	component->switch_state(to_state);
	final_ = true;

	return unique_ptr<Plan>(new Plan());
}

const Reference<platform::SwitchStateAction> &GeneralSemantics<Reference<platform::SwitchStateAction> >::element() const
{ return *element_; }

const ModelElement &GeneralSemantics<Reference<platform::SwitchStateAction> >::model_element() const
{ return element(); }

const Instruction &GeneralSemantics<Reference<platform::SwitchStateAction> >::instruction() const
{ return element(); }

AExecutionContext &GeneralSemantics<Reference<platform::SwitchStateAction> >::context() const
{ return context_; }


} // namespace gologpp
