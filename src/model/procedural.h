#ifndef PROCEDURE_H
#define PROCEDURE_H

#include <vector>
#include <memory>
#include <algorithm>

#include <boost/optional.hpp>

#include "gologpp.h"

#include "reference.h"
#include "language.h"
#include "scope.h"
#include "expressions.h"
#include "error.h"

namespace gologpp {


class Block : public Statement, public LanguageElement<Block> {
public:
	Block(const vector<Statement *> &elements, Scope &parent_scope);
	virtual void implement(Implementor &) override;

	const vector<unique_ptr<Statement>> &elements() const;

private:
	vector<unique_ptr<Statement>> elements_;
};


class Choose : public Statement, public LanguageElement<Choose> {
public:
	Choose(const vector<Statement *> &alternatives, Scope &parent_scope);
	void implement(Implementor &) override;

	const vector<unique_ptr<Statement>> &alternatives() const;

private:
	vector<unique_ptr<Statement>> alternatives_;
};


class Conditional : public Statement, public LanguageElement<Conditional> {
public:
	Conditional(unique_ptr<BooleanExpression> &&condition,
	            unique_ptr<Statement> &&block_true,
	            unique_ptr<Statement> &&block_false,
	            Scope &parent_expr);

	Conditional(BooleanExpression *condition,
	            Statement *block_true,
	            const boost::optional<Statement *> &block_false,
	            Scope &parent_expr);

	DEFINE_IMPLEMENT_WITH_MEMBERS(*condition_, *block_true_, *block_false_)

	const BooleanExpression &condition() const;
	const Statement &block_true() const;
	const Statement &block_false() const;

protected:
	unique_ptr<BooleanExpression> condition_;
	unique_ptr<Statement> block_true_;
	unique_ptr<Statement> block_false_;
};


template<class GologT>
class Assignment : public Statement, public LanguageElement<Assignment<GologT>> {
public:
	static_assert(!std::is_base_of<Statement, GologT>::value, "Cannot assign to a Statement");

	Assignment(Reference<GologT> *lhs, typename GologT::expression_t *rhs, Scope &parent_scope)
	: Statement(parent_scope)
	, lhs_(lhs)
	, rhs_(rhs)
	{}

	DEFINE_IMPLEMENT_WITH_MEMBERS(*lhs_, *rhs_)

	const Reference<GologT> &lhs() const
	{ return *lhs_; }

	const typename GologT::expression_t &rhs() const
	{ return *rhs_; }

private:
    unique_ptr<Reference<GologT>> lhs_;
    unique_ptr<typename GologT::expression_t> rhs_;
};


class Pick : public Statement, public LanguageElement<Pick> {
public:
	Pick(const shared_ptr<AbstractVariable> &variable, Statement *stmt, Scope &parent_scope);
	DEFINE_IMPLEMENT_WITH_MEMBERS(*variable_, *statement_)

	const AbstractVariable &variable() const;
	const Statement &statement() const;
	const Scope &scope() const;

private:
	shared_ptr<AbstractVariable> variable_;
	unique_ptr<Statement> statement_;
	Scope scope_;
};


class Search : public Statement, public LanguageElement<Search> {
public:
	Search(Statement *statement, Scope &parent_scope);
	DEFINE_IMPLEMENT_WITH_MEMBERS(*statement_)

	const Statement &statement() const;

private:
	unique_ptr<Statement> statement_;
};


class Test : public Statement, public LanguageElement<Test> {
public:
    Test(BooleanExpression *expression, Scope &parent_scope);
	DEFINE_IMPLEMENT_WITH_MEMBERS(*expression_)

	const BooleanExpression &expression() const;

protected:
	unique_ptr<BooleanExpression> expression_;
};


class While : public Statement, public LanguageElement<While> {
public:
	While(BooleanExpression *expression, Statement *stmt, Scope &parent_scope);
	DEFINE_IMPLEMENT_WITH_MEMBERS(*expression_, *statement_)

	const BooleanExpression &expression() const;
	const Block &block() const;

protected:
	unique_ptr<BooleanExpression> expression_;
	unique_ptr<Statement> statement_;
};


template<class ExpressionT>
class Return : public Statement, public LanguageElement<Return<ExpressionT>> {
public:
	Return(ExpressionT *expr, Scope &parent_scope)
	: Statement(parent_scope)
	, expr_(expr)
	{}

	DEFINE_IMPLEMENT_WITH_MEMBERS(*expr_)

	const ExpressionT &expression() const
	{ return *expr_; }

private:
	unique_ptr<ExpressionT> expr_;
};


class AbstractFunction : public Global, public virtual AbstractLanguageElement {
public:
	AbstractFunction(Scope *own_scope, const string &name, const vector<shared_ptr<AbstractVariable>> &args, Statement *statement);

	virtual ~AbstractFunction() override;

	const Scope &scope() const;
	const Statement &statement() const;

	virtual void compile(AExecutionContext &ctx) override;

protected:
    unique_ptr<Scope> scope_;
    unique_ptr<Statement> statement_;
    vector<shared_ptr<AbstractVariable>> args_;
};


template<class ExpressionT>
class Function
: public ExpressionT
, public AbstractFunction
, public LanguageElement<Function<ExpressionT>>
{
public:
    Function(Scope *own_scope, const string &name, const vector<shared_ptr<AbstractVariable>> &args, Statement *statement)
    : ExpressionT(Scope::global_scope())
    , AbstractFunction(own_scope, name, args, statement)
    {}

    Function(Function &&) = default;
	DEFINE_IMPLEMENT_WITH_MEMBERS(*scope_, *statement_)
};


} // namespace gologpp


#endif // PROCEDURE_H
