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

#include "fluent.h"

#include <execution/controller.h>

namespace gologpp {


InitialValue::InitialValue(const vector<Expression *> &args, Value *value)
{
	set_type(value->type());
	set_args(args);
	set_value(value);
}


InitialValue::InitialValue(const boost::optional<vector<Expression *>> &args, Value *value)
: InitialValue(args.get_value_or({}), value)
{}

const vector<unique_ptr<Expression>> &InitialValue::args() const
{ return args_; }

vector<unique_ptr<Expression>> &InitialValue::args()
{ return args_; }

void InitialValue::set_args(const vector<Expression *> &args)
{
	for (Expression *c : args) {
		c->set_parent(this);
		args_.emplace_back(c);
	}
}

const Value &InitialValue::value() const
{ return *value_; }

Value &InitialValue::value()
{ return *value_; }

void InitialValue::set_value(Value *value)
{
	value->set_parent(this);
	value_.reset(value);
}


void InitialValue::attach_semantics(SemanticsFactory &implementor)
{
	if (semantics_)
		return;
	value().attach_semantics(implementor);
	for (unique_ptr<Expression> &arg : args())
		arg->attach_semantics(implementor);

	semantics_ = implementor.make_semantics(*this);
}


string InitialValue::to_string(const string &pfx) const
{ return pfx + '(' + concat_list(args(), ", ", pfx) + ") = " + value().to_string(pfx); }

Fluent &InitialValue::fluent()
{ return *fluent_; }

const Fluent &InitialValue::fluent() const
{ return *fluent_; }

void InitialValue::set_fluent(Fluent &f)
{ fluent_ = &f; }

Scope &InitialValue::parent_scope()
{ return fluent().scope(); }

const Scope &InitialValue::parent_scope() const
{ return fluent().scope(); }



Fluent::Fluent(Scope *own_scope, const Type &t, const string &name, const vector<shared_ptr<Variable>> &args)
: Signified<Expression>(name, args)
, ScopeOwner(own_scope)
{ set_type(t); }

/*Fluent::Fluent(Scope &parent_scope, const string &name)
: Global(name, {})
, ScopeOwner(new Scope(parent_scope))
{}*/


const vector<unique_ptr<InitialValue>> &Fluent::initially() const
{ return initial_values_; }

void Fluent::define(const vector<InitialValue *> &initial_values)
{ define(boost::optional<vector<InitialValue *>>(initial_values)); }

Reference<Fluent> *Fluent::make_ref(const vector<Expression *> &args)
{ return make_ref_<Fluent>(args); }

Expression *Fluent::ref(const vector<Expression *> &args)
{ return make_ref(args); }

void Fluent::compile(AExecutionContext &ctx)
{ ctx.compile(*this); }


void Fluent::define(const boost::optional<vector<InitialValue *>> &initial_values)
{
	for (shared_ptr<Variable> &param : params())
		if (param->domain().is_implicit())
			param->define_implicit_domain("implicit_domain("
			+ param->str() + "@" + name() + "/" + std::to_string(arity()) +
			")");

	if (initial_values) {
		// TODO: fail if already defined
		for (InitialValue *ival : initial_values.get()) {
			if (!(this->type() >= ival->value()))
				throw TypeError(ival->value(), this->type());

			if (arity() != ival->args().size())
				throw UserError("Fluent " + str() + ": Arity mismatch with initial value " + ival->str());

			for (arity_t param_idx = 0; param_idx < arity(); ++param_idx) {
				Variable &param = *params()[param_idx];
				Expression &arg = *ival->args()[param_idx];

				if (!(param.type() >= arg))
					throw TypeError(arg, param.type());

				// Make sure the argument is either a value or a reference to one of this fluent's parameters
				Reference<Variable> *var_ref = dynamic_cast<Reference<Variable> *>(&arg);
				if (arg.is_a<Value>())
					param.add_implicit_domain_element(dynamic_cast<Value &>(arg));
				else if (!var_ref || *var_ref->target() != param)
					throw Bug("Fluent argument in initial value definition must be either a Value or a Reference to a parameter");
			}
			ival->set_fluent(*this);

			initial_values_.push_back(unique_ptr<InitialValue>(ival));
		}
	}
	else
		throw UserError("Fluent " + signature_str() + ": No `initially:' block");
}


void Fluent::attach_semantics(SemanticsFactory &implementor)
{
	if (semantics_)
		return;

	for (unique_ptr<InitialValue> &ival : initial_values_)
		ival->attach_semantics(implementor);
	scope_->attach_semantics(implementor);
	semantics_ = implementor.make_semantics(*this);
}


string Fluent::to_string(const string &pfx) const
{
	return linesep + pfx + type().name() + " fluent " + name() + '('
	+ concat_list(params(), ", ", "") + ") {" + linesep
	+ pfx + "initially:" + linesep
	+ pfx + concat_list(initially(), ";" linesep + pfx, pfx) + linesep
	+ pfx + '}';
}



} // namespace gologpp
