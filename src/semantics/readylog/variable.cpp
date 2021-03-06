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

#include "variable.h"
#include "reference.h"
#include "domain.h"
#include "execution.h"

namespace gologpp {


std::unordered_set<Semantics<Variable> *> Semantics<Variable>::all_vars;


Semantics<Variable>::Semantics(const Variable &var, ReadylogContext &context)
: GeneralSemantics<Variable>(var, context)
, dead_(true)
, as_golog_var_(false)
{
	const ModelElement *parent = element().parent();
	size_t level = 1;
	while (parent && parent != &element() && dynamic_cast<const ChildElement *>(parent)) {
		parent = dynamic_cast<const ChildElement *>(parent)->parent();
		++level;
	}
	golog_var_ = EC_atom(
		(element().name() + "_lv" + std::to_string(level)).c_str()
	);

	all_vars.insert(this);
}

Semantics<Variable>::~Semantics()
{
	all_vars.erase(this);
}

void Semantics<Variable>::init()
{
	ec_var_ = ::newvar();
	dead_ = false;
}

EC_word Semantics<Variable>::plterm()
{
	if (as_golog_var_)
		return golog_var_;
	else {
		if (dead_)
			init();
		return ec_var_;
	}
}


void Semantics<Variable>::translate_as_golog_var(bool gv)
{ as_golog_var_ = gv; }


EC_word Semantics<Variable>::member_restriction()
{
	return ::term(EC_functor("member", 2),
		element().special_semantics().plterm(),
		dynamic_cast<const Domain &>(element().type()).special_semantics().plterm()
	);
}



GologVarMutator::GologVarMutator(Semantics<Variable> &var_impl)
: var_impl_(var_impl)
{ var_impl_.translate_as_golog_var(true); }

GologVarMutator::~GologVarMutator()
{ var_impl_.translate_as_golog_var(false); }

GologVarMutator::GologVarMutator(const Reference<Variable> &var_ref)
: GologVarMutator(var_ref.target()->special_semantics())
{}

void Semantics<Variable>::mark_dead()
{ dead_ = true; }



} // namespace gologpp
