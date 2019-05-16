#ifndef GOLOGPP_FLUENT_H_
#define GOLOGPP_FLUENT_H_

#include "gologpp.h"
#include "utilities.h"
#include "error.h"
#include "global.h"
#include "reference.h"
#include "constant.h"

namespace gologpp {


class InitialValue
: public virtual AbstractLanguageElement
, public NoScopeOwner
, public LanguageElement<InitialValue> {
public:
	InitialValue(const vector<Constant *> &args, Constant *value)
	{
		set_type(value->type());
		set_args(args);
		set_value(value);
	}


	InitialValue(const boost::optional<vector<Constant *>> &args, Constant *value)
	: InitialValue(args.get_value_or({}), value)
	{}

	const vector<unique_ptr<Constant>> &args() const
	{ return args_; }

	vector<unique_ptr<Constant>> &args()
	{ return args_; }

	void set_args(const vector<Constant *> &args)
	{
		for (Constant *c : args) {
			c->set_parent(this);
			args_.emplace_back(c);
		}
	}

	const Constant &value() const
	{ return *value_; }

	Constant &value()
	{ return *value_; }

	void set_value(Constant *value)
	{
		value->set_parent(this);
		value_.reset(value);
	}


	virtual void attach_semantics(SemanticsFactory &implementor) override
	{
		if (semantics_)
			return;
		value().attach_semantics(implementor);
		for (unique_ptr<Constant> &arg : args())
			arg->attach_semantics(implementor);

		semantics_ = implementor.make_semantics(*this);
	}


	virtual string to_string(const string &pfx) const override
	{ return '(' + concat_list(args(), ", ", pfx) + ") = " + value().to_string(pfx); }

	Fluent &fluent()
	{ return *fluent_; }

	const Fluent &fluent() const
	{ return *fluent_; }

	void set_fluent(Fluent &f)
	{ fluent_ = &f; }

	virtual Scope &parent_scope() override;
	virtual const Scope &parent_scope() const override;

private:
	vector<unique_ptr<Constant>> args_;
	unique_ptr<Constant> value_;
	Fluent *fluent_;
};



class Fluent
: public Global
, public ScopeOwner
, public virtual AbstractLanguageElement
, public LanguageElement<Fluent>
{
public:
	Fluent(Scope *own_scope, const string &type_name, const string &name, const vector<shared_ptr<Variable>> &args);
	Fluent(Scope &parent_scope, const string &type_name, const string &name);

	virtual ~Fluent() override = default;

	const vector<unique_ptr<InitialValue>> &initially() const
	{ return initial_values_; }

	void define(const vector<InitialValue *> &initial_values)
	{ define(boost::optional<vector<InitialValue *>>(initial_values)); }

	void define(const boost::optional<vector<InitialValue *>> &initial_values)
	{
		global_scope().register_domain(domain_);

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

				domain_->add_element(ival->value());

				initial_values_.push_back(unique_ptr<InitialValue>(ival));
			}
		}
		else
			throw UserError("Fluent " + signature_str() + ": No `initially:' block");
	}


	virtual void attach_semantics(SemanticsFactory &implementor) override
	{
		if (semantics_)
			return;

		for (unique_ptr<InitialValue> &ival : initial_values_)
			ival->attach_semantics(implementor);
		scope_->attach_semantics(implementor);
		semantics_ = implementor.make_semantics(*this);
	}


	virtual string to_string(const string &pfx) const override
	{
		return linesep + pfx + gologpp::to_string(type().dynamic_tag()) + "fluent " + name() + '('
			+ concat_list(args(), ", ", "") + ") {" + linesep
			+ pfx + "initially:" + linesep
			+ pfx + concat_list(initially(), ";" linesep + pfx, pfx) + linesep
			+ pfx + '}';
	}

	Reference<Fluent> *make_ref(const vector<Expression *> &args)
	{ return make_ref_<Fluent>(args); }

	virtual Expression *ref(const vector<Expression *> &args) override
	{ return make_ref(args); }

	virtual void compile(AExecutionContext &ctx) override;

private:
	vector<unique_ptr<InitialValue>> initial_values_;
	shared_ptr<Domain> domain_;
};



} // namespace gologpp



#endif // GOLOGPP_FLUENT_H_
