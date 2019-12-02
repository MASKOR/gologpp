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

#ifndef GOLOGPP_FLUENT_H_
#define GOLOGPP_FLUENT_H_

#include "gologpp.h"
#include "utilities.h"
#include "error.h"
#include "global.h"
#include "reference.h"
#include "value.h"

namespace gologpp {


class InitialValue
: public virtual AbstractLanguageElement
, public NoScopeOwner
, public LanguageElement<InitialValue> {
public:
	InitialValue(const vector<Expression *> &args, Value *value);
	InitialValue(const boost::optional<vector<Expression *>> &args, Value *value);

	const vector<unique_ptr<Expression>> &args() const;
	vector<unique_ptr<Expression>> &args();
	void set_args(const vector<Expression *> &args);

	const Value &value() const;
	Value &value();
	void set_value(Value *value);

	Fluent &fluent();
	const Fluent &fluent() const;
	void set_fluent(Fluent &f);

	virtual Scope &parent_scope() override;
	virtual const Scope &parent_scope() const override;

	virtual void attach_semantics(SemanticsFactory &implementor) override;

	virtual string to_string(const string &pfx) const override;

private:
	vector<unique_ptr<Expression>> args_;
	unique_ptr<Value> value_;
	Fluent *fluent_;
};



class Fluent
: public Signified<Expression>
, public ScopeOwner
, public virtual AbstractLanguageElement
, public LanguageElement<Fluent>
{
public:
	Fluent(Scope *own_scope, const string &type_name, const string &name, const vector<shared_ptr<Variable>> &params);
	//Fluent(Scope &parent_scope, const string &name);

	virtual ~Fluent() override = default;

	const vector<unique_ptr<InitialValue>> &initially() const;
	bool synced() const;
	const ExogAction &sync_action() const;
	ExogAction &sync_action();

	void define(const vector<InitialValue *> &initial_values, bool synced = false);
	void define(const boost::optional<vector<InitialValue *>> &initial_values, bool synced = false);

	Reference<Fluent> *make_ref(const vector<Expression *> &params);
	virtual Reference<Fluent> *ref(const vector<Expression *> &params) override;

	virtual void attach_semantics(SemanticsFactory &implementor) override;
	virtual string to_string(const string &pfx) const override;
	virtual void compile(AExecutionContext &ctx) override;

private:
	vector<unique_ptr<InitialValue>> initial_values_;
	shared_ptr<ExogAction> sync_action_;
};



} // namespace gologpp



#endif // GOLOGPP_FLUENT_H_
