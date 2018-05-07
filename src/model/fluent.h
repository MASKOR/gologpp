#ifndef GOLOGPP_FLUENT_H_
#define GOLOGPP_FLUENT_H_

#include <string>
#include <memory>
#include <algorithm>

#include "gologpp.h"
#include "atoms.h"
#include "utilities.h"
#include "reference.h"
#include "scope.h"
#include "arithmetic.h"
#include "error.h"

namespace gologpp {


class AbstractFluent
: public Expression
, public Identifier
, public std::enable_shared_from_this<AbstractFluent>
, public virtual AbstractLanguageElement
{
public:
	AbstractFluent(const string &name, const vector<string> &args);
	AbstractFluent(AbstractFluent &&) = default;

	virtual ~AbstractFluent() override = default;

	const vector<string> &args() const;

	shared_ptr<AbstractVariable> argument(arity_t idx) const;

	Scope &scope();
	const Scope &scope() const;

	template<class ExpressionT>
	void declare_variable(const string &name)
	{
		if (std::find(args_.begin(), args_.end(), name) == args_.end())
			throw Bug("Fluent `" + this->name() + "' has no argument named `" + name + "'");
		scope_.variable<ExpressionT>(name);
	}

protected:
	Scope scope_;
	vector<string> args_;
};


template<class ExpressionT>
class Fluent
: public ExpressionT
, public AbstractFluent
, public std::enable_shared_from_this<Fluent<ExpressionT>>
, public LanguageElement<Fluent<ExpressionT>>
{
public:
	Fluent(const string &name, const vector<string> &args, unique_ptr<ExpressionT> &&init)
	: ExpressionT(Scope::global_scope())
	, AbstractFluent(name, args)
	, initial_value_(std::move(init))
	{}

	Fluent(Fluent &&) = default;

	virtual ~Fluent() override = default;

	DEFINE_IMPLEMENT_WITH_MEMBERS(scope_, *initial_value_)

private:
	unique_ptr<ExpressionT> initial_value_;
};





} // namespace gologpp



#endif // GOLOGPP_FLUENT_H_
