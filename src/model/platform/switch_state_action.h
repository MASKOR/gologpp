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

#pragma once

#include <model/action.h>

namespace gologpp {
namespace platform {


class SwitchStateAction
: public AbstractAction
, public LanguageElement<SwitchStateAction> {
public:
	SwitchStateAction(Scope *own_scope);

	virtual void compile(AExecutionController &) override;
	virtual void attach_semantics(gologpp::SemanticsFactory &) override;
	virtual string to_string(const string &pfx) const override;
	virtual Instruction *ref(const vector<Expression *> &args) override;
	gologpp::Reference<SwitchStateAction> *make_ref(const vector<Expression *> &args);
	virtual void set_mapping (BackendMapping *) override;

	shared_ptr<const Variable> param_component() const;
	shared_ptr<const Variable> param_from_state() const;
	shared_ptr<const Variable> param_to_state() const;

private:
	shared_ptr<const Variable> param_component_, param_from_state_, param_to_state_;
};


} // namespace platform

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

template<>
class GeneralSemantics<Reference<platform::SwitchStateAction>>
: public virtual GeneralSemantics<Instruction>
{
public:
	GeneralSemantics(const Reference<platform::SwitchStateAction> &elem, AExecutionController &context);

	virtual bool final(const BindingChain &b, const History &h) override;
	virtual unique_ptr<Plan> trans(const BindingChain &, History &) override;

	const Reference<platform::SwitchStateAction> &element() const;
	virtual const ModelElement &model_element() const override;
	virtual const Instruction &instruction() const override;

	virtual AExecutionController &context() const override;

private:
	const Reference<platform::SwitchStateAction> *element_;
	AExecutionController &context_;
	bool final_;
};



} // namespace gologpp
