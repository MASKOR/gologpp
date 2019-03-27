#ifndef GOLOGPP_FLUENT_H_
#define GOLOGPP_FLUENT_H_

#include "gologpp.h"
#include "utilities.h"
#include "error.h"
#include "global.h"
#include "reference.h"
#include "constant.h"

namespace gologpp {


template<class ExpressionT>
class InitialValue
: public virtual AbstractLanguageElement
, public NoScopeOwner
, public LanguageElement<InitialValue<ExpressionT>> {
public:
	InitialValue(const vector<AbstractConstant *> &args, Constant<ExpressionT> *value)
	{
		set_args(args);
		set_value(value);
	}


	InitialValue(const boost::optional<vector<AbstractConstant *>> &args, Constant<ExpressionT> *value)
	: InitialValue(args.get_value_or({}), value)
	{}

	const vector<unique_ptr<AbstractConstant>> &args() const
	{ return args_; }

	vector<unique_ptr<AbstractConstant>> &args()
	{ return args_; }

	void set_args(const vector<AbstractConstant *> &args)
	{
		for (AbstractConstant *c : args) {
			dynamic_cast<Expression *>(c)->set_parent(this);
			args_.emplace_back(c);
		}
	}

	const Constant<ExpressionT> &value() const
	{ return *value_; }

	Constant<ExpressionT> &value()
	{ return *value_; }

	void set_value(Constant<ExpressionT> *value)
	{
		value->set_parent(this);
		value_.reset(value);
	}


	virtual void attach_semantics(SemanticsFactory &implementor) override
	{
		if (semantics_)
			return;
		value().attach_semantics(implementor);
		for (unique_ptr<AbstractConstant> &arg : args())
			arg->attach_semantics(implementor);

		semantics_ = implementor.make_semantics(*this);
	}


	virtual string to_string(const string &pfx) const override
	{ return '(' + concat_list(args(), ", ", pfx) + ") = " + value().to_string(pfx); }

	Fluent<ExpressionT> &fluent()
	{ return *fluent_; }

	const Fluent<ExpressionT> &fluent() const
	{ return *fluent_; }

	void set_fluent(Fluent<ExpressionT> &f)
	{ fluent_ = &f; }

	virtual Scope &parent_scope() override
	{ return fluent().scope(); }

	virtual const Scope &parent_scope() const override
	{ return fluent().scope(); }

private:
	vector<unique_ptr<AbstractConstant>> args_;
	unique_ptr<Constant<ExpressionT>> value_;
	Fluent<ExpressionT> *fluent_;
};



class AbstractFluent
: public Global
, public ScopeOwner
, public virtual AbstractLanguageElement
{
public:
	typedef Expression expression_t;
	typedef AbstractFluent abstract_t;

	AbstractFluent(Scope *own_scope, const string &name, const vector<shared_ptr<AbstractVariable>> &args);

	virtual ~AbstractFluent() override = default;
	virtual ExpressionTypeTag expression_type_tag() const = 0;

	virtual void compile(AExecutionContext &ctx) override;
};


template<class ExpressionT>
class Fluent
: public AbstractFluent
, public LanguageElement<Fluent<ExpressionT>>
{
public:
	typedef ExpressionT expression_t;
	typedef AbstractFluent abstract_t;
	typedef typename ExpressionT::type_t type_t;

	Fluent(Scope *own_scope, const string &name, const vector<shared_ptr<AbstractVariable>> &args)
	: AbstractFluent(own_scope, name, args)
	, domain_(new Domain<ExpressionT>(
		"implicit_domain(" + name + "/" + std::to_string(args.size()) + ")",
		{},
		true
	  ) )
	{ domain_->add_subject(*this); }

	Fluent(Scope &parent_scope, const string &name)
	: AbstractFluent(new Scope(parent_scope), name, {})
	{}

	virtual ~Fluent() override = default;

	virtual ExpressionTypeTag expression_type_tag() const override
	{ return ExpressionT::static_type_tag(); }

	const vector<unique_ptr<InitialValue<ExpressionT>>> &initially() const
	{ return initial_values_; }

	void define(const vector<InitialValue<ExpressionT> *> &initial_values)
	{ define(boost::optional<vector<InitialValue<ExpressionT> *>>(initial_values)); }

	void define(const boost::optional<vector<InitialValue<ExpressionT> *>> &initial_values)
	{
		global_scope().register_domain(domain_);

		for (shared_ptr<AbstractVariable> &arg : args())
			if (arg->domain().is_implicit())
				arg->define_implicit_domain("implicit_domain("
					+ arg->str() + "@" + name() + "/" + std::to_string(arity()) +
				")");

		if (initial_values) {
			// TODO: fail if already defined
			for (InitialValue<ExpressionT> *ival : initial_values.get()) {
				if (arity() != ival->args().size())
					throw UserError("Fluent " + str() + ": Arity mismatch with initial value " + ival->str());

				for (arity_t arg_idx = 0; arg_idx < arity(); ++arg_idx) {
					AbstractVariable &arg = *args()[arg_idx];
					AbstractConstant &arg_value = *ival->args()[arg_idx];
					if (arg_value.dynamic_type_tag() != arg.dynamic_type_tag())
						throw ExpressionTypeMismatch(
							dynamic_cast<const Expression &>(arg),
							dynamic_cast<const Expression &>(arg_value)
						);

					arg.add_implicit_domain_element(arg_value);
				}
				ival->set_fluent(*this);

				domain_->add_element(ival->value());

				initial_values_.push_back(unique_ptr<InitialValue<ExpressionT>>(ival));
			}
		}
		else
			throw UserError("Fluent " + signature_str() + ": No `initially:' block");
	}


	virtual void attach_semantics(SemanticsFactory &implementor) override
	{
		if (semantics_)
			return;

		for (unique_ptr<InitialValue<ExpressionT>> &ival : initial_values_)
			ival->attach_semantics(implementor);
		scope_->attach_semantics(implementor);
		semantics_ = implementor.make_semantics(*this);
	}


	virtual string to_string(const string &pfx) const override
	{
		return linesep + pfx + gologpp::to_string(expression_type_tag()) + "fluent " + name() + '('
			+ concat_list(args(), ", ", "") + ") {" + linesep
			+ pfx + "initially:" + linesep
			+ pfx + concat_list(initially(), ";" linesep + pfx, pfx) + linesep
			+ pfx + '}';
	}

	Reference<Fluent<ExpressionT>> *make_ref(const vector<Expression *> &args)
	{ return make_ref_<Fluent<ExpressionT>>(args); }

	virtual Expression *ref(const vector<Expression *> &args) override
	{ return make_ref(args); }

private:
	vector<unique_ptr<InitialValue<ExpressionT>>> initial_values_;
	shared_ptr<Domain<ExpressionT>> domain_;
};



} // namespace gologpp



#endif // GOLOGPP_FLUENT_H_
