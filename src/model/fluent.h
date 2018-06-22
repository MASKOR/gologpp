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
: public Global
, public virtual AbstractLanguageElement
{
public:
	AbstractFluent(Scope *own_scope, const string &name, const vector<shared_ptr<AbstractVariable>> &args);
	AbstractFluent(AbstractFluent &&) = default;

	virtual ~AbstractFluent() override = default;

	Scope &scope();
	const Scope &scope() const;

protected:
	unique_ptr<Scope> scope_;
};


template<class ExpressionT>
class Fluent
: public ExpressionT
, public AbstractFluent
, public LanguageElement<Fluent<ExpressionT>>
{
public:
	Fluent(Scope *own_scope, const string &name, const vector<shared_ptr<AbstractVariable>> &args, unique_ptr<ExpressionT> &&init)
	: ExpressionT(Scope::global_scope())
	, AbstractFluent(own_scope, name, args)
	, initial_value_(std::move(init))
	{}

	Fluent(Fluent &&) = default;

	virtual ~Fluent() override = default;

	DEFINE_IMPLEMENT_WITH_MEMBERS(*scope_, *initial_value_)

private:
	unique_ptr<ExpressionT> initial_value_;
};



} // namespace gologpp



#endif // GOLOGPP_FLUENT_H_
