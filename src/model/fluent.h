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
#include "value.h"

namespace gologpp {


class InitialValue
: public virtual ModelElement
, public NoScopeOwner
, public LanguageElement<InitialValue> {
public:
	using ElementType = ModelElement;

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
, public virtual ModelElement
, public LanguageElement<Fluent>
{
public:
	Fluent(Scope *own_scope, const Type &t, const string &name, const vector<shared_ptr<Variable>> &params);
	//Fluent(Scope &parent_scope, const string &name);

	virtual ~Fluent() override = default;

	const vector<unique_ptr<InitialValue>> &initially() const;

	void define(const vector<InitialValue *> &initial_values);
	void define(const boost::optional<vector<InitialValue *>> &initial_values);

	Reference<Fluent> *make_ref(const vector<Expression *> &params);
	virtual Expression *ref(const vector<Expression *> &params) override;

	virtual void attach_semantics(SemanticsFactory &implementor) override;

	virtual string to_string(const string &pfx) const override;

	virtual void compile(AExecutionController &ctx) override;

private:
	vector<unique_ptr<InitialValue>> initial_values_;
};



} // namespace gologpp



#endif // GOLOGPP_FLUENT_H_
