#ifndef GOLOGPP_FLUENT_H_
#define GOLOGPP_FLUENT_H_

#include <string>
#include <memory>

#include "gologpp.h"
#include "atoms.h"
#include "utilities.h"
#include "Reference.h"
#include "Scope.h"
#include "arithmetic.h"

namespace gologpp {


class AbstractFluent
: public Identifier
, public std::enable_shared_from_this<AbstractFluent>
, public AbstractLanguageElement
{
public:
	AbstractFluent(const string &name, const vector<string> &args)
	: Identifier(name, static_cast<arity_t>(args.size()))
	, scope_(args, Scope::global_scope())
	, args_(args)
	{}

	virtual ~AbstractFluent();

	const vector<string> &args() const
	{ return args_; }

	Scope &scope()
	{ return scope_; }
	const Scope &scope() const
	{ return scope_; }

protected:
	Scope scope_;
	vector<string> args_;
};


template<class ExpressionT>
class Fluent
: public ExpressionT
, public Identifier
, public std::enable_shared_from_this<Fluent<ExpressionT>>
, public LanguageElement<Fluent<ExpressionT>>
{
public:
	Fluent(const string &name, const vector<string> &args, unique_ptr<ExpressionT> &&init)
	: ExpressionT(Scope::global_scope())
	, Identifier(name, static_cast<arity_t>(args.size()))
	, initial_value_(std::move(init))
	{}

	virtual ~Fluent() override = default;

private:
	unique_ptr<ExpressionT> initial_value_;
};





} // namespace gologpp



#endif // GOLOGPP_FLUENT_H_
