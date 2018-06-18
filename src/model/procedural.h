#ifndef PROCEDURE_H
#define PROCEDURE_H

#include <vector>
#include <memory>
#include <algorithm>

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

	DEFINE_IMPLEMENT_WITH_MEMBERS(*condition_, *block_true_, *block_false_)

	const BooleanExpression &condition() const;
	const Statement &block_true() const;
	const Statement &block_false() const;

protected:
	unique_ptr<BooleanExpression> condition_;
	unique_ptr<Statement> block_true_;
	unique_ptr<Statement> block_false_;
};


template<class ExpressionT>
class Assignment : public Statement, public LanguageElement<Assignment<ExpressionT>> {
public:
	Assignment(Reference<Fluent<ExpressionT>> &&fluent, unique_ptr<ExpressionT> &&expression, Scope &parent_scope)
	: Statement(parent_scope)
	, fluent_(std::move(fluent))
	, expression_(std::move(expression))
	{}

	DEFINE_IMPLEMENT_WITH_MEMBERS(fluent_, *expression_)

	const Reference<Fluent<ExpressionT>> &fluent() const
	{ return fluent_; }

	const Expression &expression() const
	{ return *expression_; }


private:
    Reference<Fluent<ExpressionT>> fluent_;
    unique_ptr<ExpressionT> expression_;
};


class Pick : public Statement, public LanguageElement<Pick> {
public:
	Pick(const shared_ptr<AbstractVariable> &variable, Block &&block, Scope &parent_scope);
	DEFINE_IMPLEMENT_WITH_MEMBERS(*variable_, block_)

	const AbstractVariable &variable() const;
	const Block &block() const;
	const Scope &scope() const;

private:
	shared_ptr<AbstractVariable> variable_;
	Block block_;
	Scope scope_;
};


class Search : public Statement, public LanguageElement<Search> {
public:
	Search(Block &&block, Scope &parent_scope);
	DEFINE_IMPLEMENT_WITH_MEMBERS(block_)

	const Block &block() const;

private:
	Block block_;
};


class Test : public Statement, public LanguageElement<Test> {
public:
    Test(unique_ptr<BooleanExpression> &&expression, Scope &parent_scope);
	DEFINE_IMPLEMENT_WITH_MEMBERS(*expression_)

	const BooleanExpression &expression() const;

protected:
	unique_ptr<BooleanExpression> expression_;
};


class While : public Statement, public LanguageElement<While> {
public:
	While(unique_ptr<BooleanExpression> &&expression, Block &&block, Scope &parent_scope);
	DEFINE_IMPLEMENT_WITH_MEMBERS(*expression_, block_)

	const BooleanExpression &expression() const;
	const Block &block() const;

protected:
	unique_ptr<BooleanExpression> expression_;
	Block block_;
};


template<class ExpressionT>
class Return : public Statement, public LanguageElement<Return<ExpressionT>> {
public:
	Return(unique_ptr<ExpressionT> &&expr, Scope &parent_scope)
	: ExpressionT(parent_scope)
	, expr_(expr)
	{}

	const ExpressionT &expression() const
	{ return *expr_; }

private:
	unique_ptr<ExpressionT> expr_;
};


class AbstractFunction : public Global, public virtual AbstractLanguageElement {
public:
	AbstractFunction(const string &name, const vector<shared_ptr<AbstractVariable>> &args, Block &&block);

	virtual ~AbstractFunction() override;

	const Scope &scope() const;
	const Block &block() const;

protected:
    Scope scope_;
    Block block_;
    vector<shared_ptr<AbstractVariable>> args_;
};


template<class ExpressionT>
class Function
: public ExpressionT
, public AbstractFunction
, public LanguageElement<Function<ExpressionT>>
{
public:
    Function(const string &name, const vector<shared_ptr<AbstractVariable>> &args, Block &&block)
    : ExpressionT(Scope::global_scope())
    , AbstractFunction(name, args, std::move(block))
    {}

    Function(Function &&) = default;
	DEFINE_IMPLEMENT_WITH_MEMBERS(scope_, block_)
};


} // namespace gologpp


#endif // PROCEDURE_H
