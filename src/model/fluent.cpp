#include "fluent.h"
#include "execution.h"

namespace gologpp {


InitialValue::InitialValue(const vector<Constant *> &args, Constant *value)
{
	set_type(value->type());
	set_args(args);
	set_value(value);
}


InitialValue::InitialValue(const boost::optional<vector<Constant *>> &args, Constant *value)
: InitialValue(args.get_value_or({}), value)
{}

const vector<unique_ptr<Constant>> &InitialValue::args() const
{ return args_; }

vector<unique_ptr<Constant>> &InitialValue::args()
{ return args_; }

void InitialValue::set_args(const vector<Constant *> &args)
{
	for (Constant *c : args) {
		c->set_parent(this);
		args_.emplace_back(c);
	}
}

const Constant &InitialValue::value() const
{ return *value_; }

Constant &InitialValue::value()
{ return *value_; }

void InitialValue::set_value(Constant *value)
{
	value->set_parent(this);
	value_.reset(value);
}


void InitialValue::attach_semantics(SemanticsFactory &implementor)
{
	if (semantics_)
		return;
	value().attach_semantics(implementor);
	for (unique_ptr<Constant> &arg : args())
		arg->attach_semantics(implementor);

	semantics_ = implementor.make_semantics(*this);
}


string InitialValue::to_string(const string &pfx) const
{ return '(' + concat_list(args(), ", ", pfx) + ") = " + value().to_string(pfx); }

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



Fluent::Fluent(Scope *own_scope, const string &name, const vector<shared_ptr<Variable>> &args)
: Global(name, args)
, ScopeOwner(own_scope)
{}

Fluent::Fluent(Scope &parent_scope, const string &name)
: Global(name, {})
, ScopeOwner(new Scope(parent_scope))
{}


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
	for (shared_ptr<Variable> &arg : args())
		if (arg->domain().is_implicit())
			arg->define_implicit_domain("implicit_domain("
			+ arg->str() + "@" + name() + "/" + std::to_string(arity()) +
			")");

	if (initial_values) {
		// TODO: fail if already defined
		for (InitialValue *ival : initial_values.get()) {
			ensure_type_equality(*this, *ival);
			if (arity() != ival->args().size())
				throw UserError("Fluent " + str() + ": Arity mismatch with initial value " + ival->str());

			for (arity_t arg_idx = 0; arg_idx < arity(); ++arg_idx) {
				Variable &arg = *args()[arg_idx];
				Constant &arg_value = *ival->args()[arg_idx];

				ensure_type_equality(arg, arg_value);

				arg.add_implicit_domain_element(arg_value);
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
	+ concat_list(args(), ", ", "") + ") {" + linesep
	+ pfx + "initially:" + linesep
	+ pfx + concat_list(initially(), ";" linesep + pfx, pfx) + linesep
	+ pfx + '}';
}



} // namespace gologpp
