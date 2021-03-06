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

#include "mapping.h"
#include "action.h"
#include "fluent.h"
#include "reference.h"
#include "effect_axiom.h"
#include "execution/controller.h"
#include "error.h"
#include <string>

namespace gologpp {


AbstractAction::AbstractAction(
	Scope *own_scope,
	const Type &, // ignored type
	const string &name,
	const vector<shared_ptr<Variable>> &params
)
: Signified<Instruction>(name, params)
, ScopeOwner(own_scope)
, mapping_(new BackendMapping(*this))
{
	set_type(gologpp::type<VoidType>());
	set_precondition(new Value(gologpp::type<BoolType>(), true));
	set_silent(false);
}


const vector<unique_ptr<AbstractEffectAxiom>> &AbstractAction::effects() const
{ return effects_; }


void AbstractAction::add_effect(AbstractEffectAxiom *effect)
{
	effect->set_action(*this);
	effects_.emplace_back(effect);
}


string AbstractAction::to_string(const string &pfx) const
{
	return name() + '(' + concat_list(params(), ", ", "") + ")" + " {"
		+ linesep + pfx + "precondition:" + linesep + precondition().to_string(pfx + indent)
		+ linesep + pfx + "effect:" + linesep + concat_list(effects(), ";" linesep, pfx) + linesep "}";
}

bool AbstractAction::operator != (const AbstractAction &other) const
{ return !(*this == other); }

bool AbstractAction::operator == (const AbstractAction &other) const
{ return typeid(*this) == typeid(other) && hash() == other.hash(); }

const Expression &AbstractAction::precondition() const
{ return *precondition_; }

Expression &AbstractAction::precondition()
{ return *precondition_; }


void AbstractAction::set_precondition(Expression *cond)
{
	precondition_ = cond;
	precondition_->set_parent(this);
}

const BackendMapping& AbstractAction::mapping() const
{ return *mapping_; }

BackendMapping& AbstractAction::mapping()
{ return *mapping_; }

void AbstractAction::set_mapping(BackendMapping *mapping)
{
	mapping_.reset(mapping);
	mapping_->set_parent(this);
}


void AbstractAction::attach_semantics(SemanticsFactory &implementor)
{
	scope().attach_semantics(implementor);
	precondition_->attach_semantics(implementor);
	for (unique_ptr<AbstractEffectAxiom> &effect : effects_)
		effect->attach_semantics(implementor);

	for (auto &pair : mapping_->arg_mapping())
		pair.second->attach_semantics(implementor);
}

void AbstractAction::set_silent(bool silent)
{ silent_ = silent; }

void AbstractAction::set_silent_v(boost::optional<Value *> silent)
{
	if (silent.is_initialized())
		set_silent(static_cast<bool>(*silent.get()));
}


bool AbstractAction::silent() const
{ return silent_; }

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

void Action::compile(AExecutionController &ctx)
{ ctx.compile(*this); }

void Action::attach_semantics(SemanticsFactory &f)
{
	if (!semantics_) {
		semantics_ = f.make_semantics(*this);
		AbstractAction::attach_semantics(f);
		if (senses_)
			senses_->attach_semantics(f);
	}
}

string Action::to_string(const string &pfx) const
{ return linesep + pfx + "action " + AbstractAction::to_string(pfx); }

Reference<Action> *Action::make_ref(const vector<Expression *> &args)
{ return make_ref_<Action>(args); }

Instruction *Action::ref(const vector<Expression *> &args)
{ return make_ref(args); }

void Action::set_senses(AbstractAssignment *f)
{
	senses_.reset(f);
	senses_->set_parent(this);
}

unique_ptr<AbstractAssignment> &Action::senses()
{ return senses_; }

const unique_ptr<AbstractAssignment> &Action::senses() const
{ return senses_; }

Clock::DurationRange Action::duration() const
{ return duration_; }


void Action::set_duration(boost::optional<Clock::DurationRange> dr)
{
	if (dr)
		duration_ = dr.get();
}


void Action::define(
	boost::optional<Expression *> precondition,
	boost::optional<vector<AbstractEffectAxiom *>> effects,
	boost::optional<AbstractAssignment *> senses,
	boost::optional<BackendMapping *> mapping
) {
	if (precondition)
		set_precondition(precondition.value());
	if (effects)
		for (AbstractEffectAxiom *e : *effects)
			add_effect(e);
	if (senses)
		set_senses(senses.value());
	if (mapping)
		set_mapping(mapping.value());
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

void ExogAction::compile(AExecutionController &ctx)
{ ctx.compile(*this); }

void ExogAction::attach_semantics(SemanticsFactory &f)
{
	if (!semantics_) {
		semantics_ = f.make_semantics(*this);
		AbstractAction::attach_semantics(f);
	}
}

string ExogAction::to_string(const string &pfx) const
{ return linesep + pfx + "exog_action " + AbstractAction::to_string(pfx); }

Reference<ExogAction> *ExogAction::make_ref(const vector<Expression *> &args)
{ return make_ref_<ExogAction>(args); }

Instruction *ExogAction::ref(const vector<Expression *> &args)
{ return make_ref(args); }


void ExogAction::define(
	boost::optional<Expression *> precondition,
	boost::optional<vector<AbstractEffectAxiom *>> effects,
	boost::optional<BackendMapping *> mapping
) {
	if (precondition)
		set_precondition(precondition.value());
	if (effects)
		for (AbstractEffectAxiom *e : *effects)
			add_effect(e);
	if (mapping)
		set_mapping(mapping.value());
}

void ExogAction::set_mapping(BackendMapping *m) {
	string missing;

	const BackendMapping::ArgMapping &arg_mapping = m->arg_mapping();

	for (const shared_ptr<Variable> &param : params()) {
		auto it = arg_mapping.begin();
		for (; it != arg_mapping.end(); ++it)
			if (dynamic_cast<Reference<Variable> &>(*it->second).target() == param)
				break;
		if (it == arg_mapping.end())
			missing += param->str() + ", ";
	}

	if (missing.size()) {
		missing = missing.substr(0, missing.length() - 2);
		throw UserError(signature_str() + ": Parameters missing from mapping: " + missing);
	}
	else
		AbstractAction::set_mapping(m);
}




} // namespace gologpp
