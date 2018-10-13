#ifndef GOLOGPP_VARIABLE_H_
#define GOLOGPP_VARIABLE_H_

#include "gologpp.h"
#include "language.h"
#include "utilities.h"
#include "expressions.h"
#include "scope.h"
#include "domain.h"

namespace gologpp {


class AbstractVariable : public Identifier, public virtual AbstractLanguageElement {
public:
	AbstractVariable(const string &name);

	virtual ExpressionTypeTag dynamic_type_tag() const = 0;

	template<class ExpressionT>
	operator Variable<ExpressionT> *() {
		return dynamic_cast<Variable<ExpressionT> *>(this);
	}

	virtual AbstractDomain &domain() = 0;
	virtual const AbstractDomain &domain() const = 0;

	virtual void add_to_domain(const AbstractConstant &) = 0;

	template<class ExpressionT>
	operator ExpressionT *() {
		return static_cast<ExpressionT *>(dynamic_cast<Variable<ExpressionT> *>(this));
	}
};



template<class ExpressionT>
class Variable
: public ExpressionT
, public LanguageElement<Variable<ExpressionT>>
, public AbstractVariable
, public NoScopeOwner
, public std::enable_shared_from_this<Variable<ExpressionT>>
{
protected:
	Variable(const string &name)
	: AbstractVariable(name)
	, domain_(new Domain<ExpressionT>(name, {}, true))
	{ domain_->add_subject(*this); }

public:
	Variable(Variable<ExpressionT> &&) = default;
	Variable(const Variable<ExpressionT> &) = delete;
	Variable<ExpressionT> &operator = (Variable<ExpressionT> &&) = default;
	Variable<ExpressionT> &operator = (const Variable<ExpressionT> &) = delete;

	friend Scope;

	virtual ExpressionTypeTag dynamic_type_tag() const override
	{ return ExpressionT::dynamic_type_tag(); }

	Reference<Variable<ExpressionT>> *ref()
	{
		return new Reference<Variable<ExpressionT>>(
			this->shared_from_this()
		);
	}

	virtual Domain<ExpressionT> &domain() override
	{ return *domain_; }

	virtual Domain<ExpressionT> &domain() const override
	{ return *domain_; }

	virtual void add_to_domain(const AbstractConstant &c) override
	{
		domain_->elements().emplace(new Constant<ExpressionT>(
			dynamic_cast<const Constant<ExpressionT> &>(c)
		) );
	}

	void set_domain(const string &domain_name)
	{
		domain_ = global_scope().lookup_domain<ExpressionT>(domain_name, 0);
		domain_->add_subject(*this);
	}

	virtual ~Variable() override = default;
	DEFINE_IMPLEMENT_WITH_MEMBERS(*domain_)

	virtual string to_string(const string &) const override
	{ return gologpp::to_string(dynamic_type_tag()) + name(); }

private:
	shared_ptr<Domain<ExpressionT>> domain_;
};


} // namespace gologpp



namespace std {

template<>
struct hash<shared_ptr<gologpp::AbstractVariable>> {
	size_t operator () (const shared_ptr<gologpp::AbstractVariable> &o) const
	{ return o->hash(); }
};

} // namespace std



#endif // GOLOGPP_VARIABLE_H_
