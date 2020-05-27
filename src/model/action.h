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

#ifndef GOLOGPP_ACTION_H_
#define GOLOGPP_ACTION_H_

#include <vector>
#include <memory>
#include <algorithm>

#include <boost/optional.hpp>

#include "gologpp.h"

#include "language.h"
#include "scope.h"
#include "global.h"
#include "mapping.h"
#include "reference.h"
#include "fluent.h"

namespace gologpp {


class AbstractEffectAxiom;


class AbstractAction
: public Signified<Instruction>
, public ScopeOwner
, public virtual ModelElement {
public:
	typedef AbstractAction abstract_t;

	AbstractAction(
		Scope *own_scope,
		const Type &,
		const string &name,
		const vector<shared_ptr<Variable>> &params
	);

	virtual ~AbstractAction() override = default;

	virtual bool operator == (const AbstractAction &other) const;
	bool operator != (const AbstractAction &other) const;

	const Expression &precondition() const;
	Expression &precondition();

	void set_precondition(Expression *);

	const vector<unique_ptr<AbstractEffectAxiom>> &effects() const;
	vector<unique_ptr<AbstractEffectAxiom>> &effects();
	void add_effect(AbstractEffectAxiom *effect);

	virtual void compile(AExecutionContext &ctx) override;
	virtual string to_string(const string &pfx) const override;

	const ActionMapping &mapping() const;
	ActionMapping &mapping();
	virtual void set_mapping (ActionMapping *);

	virtual void attach_semantics(SemanticsFactory &) override;

	void set_silent(bool);
	void set_silent_v(boost::optional<Value *>);
	bool silent() const;

private:
	vector<unique_ptr<AbstractEffectAxiom>> effects_;
	SafeExprOwner<BoolType> precondition_;
	unique_ptr<ActionMapping> mapping_;
	bool silent_;
};



class Action : public AbstractAction, public LanguageElement<Action> {
public:
	using AbstractAction::AbstractAction;

	void set_senses(Reference<Fluent> *);

	void define(
		boost::optional<Expression *> precondition,
		boost::optional<vector<AbstractEffectAxiom *>> effects,
		boost::optional<Reference<Fluent> *> senses,
		boost::optional<ActionMapping *> mapping
	);

	virtual void attach_semantics(SemanticsFactory &) override;
	virtual string to_string(const string &pfx) const override;
	virtual Instruction *ref(const vector<Expression *> &params) override;
	Reference<Action> *make_ref(const vector<Expression *> &params);

	unique_ptr<Reference<Fluent>> &senses();
	const unique_ptr<Reference<Fluent>> &senses() const;

private:
	unique_ptr<Reference<Fluent>> senses_;
};



class ExogAction : public AbstractAction, public LanguageElement<ExogAction> {
public:
	using AbstractAction::AbstractAction;

	void define(
		boost::optional<Expression *> precondition,
		boost::optional<vector<AbstractEffectAxiom *>> effects,
		boost::optional<ActionMapping *> mapping
	);

	virtual void attach_semantics(SemanticsFactory &) override;
	virtual string to_string(const string &pfx) const override;
	virtual Instruction *ref(const vector<Expression *> &params) override;
	Reference<ExogAction> *make_ref(const vector<Expression *> &params);
	virtual void set_mapping (ActionMapping *) override;
};



} // namespace gologpp




#endif /* GOLOGPP_ACTION_H_ */
