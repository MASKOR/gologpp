#ifndef PROCEDURE_H
#define PROCEDURE_H

#include <vector>
#include <memory>
#include <algorithm>

#include "gologpp.h"

#include "Reference.h"
#include "Language.h"
#include "Scope.h"
#include "expressions.h"
#include "error.h"

namespace gologpp {


class Block : public Statement, public LanguageElement<Block> {
public:
	Block(vector<unique_ptr<Statement>> &&elements, Scope &parent_scope);
	void implement(Implementor &);

	const vector<unique_ptr<Statement>> &elements() const;

private:
	vector<unique_ptr<Statement>> elements_;
};


class Choose : public Statement, public LanguageElement<Choose> {
public:
	Choose(vector<Block> &&alternatives, Scope &parent_scope);
	void implement(Implementor &);

	const vector<Block> &alternatives() const;

private:
	vector<Block> alternatives_;
};


class Conditional : public Statement, public LanguageElement<Conditional> {
public:
	Conditional(unique_ptr<BooleanExpression> &&condition,
	            Block &&block_true,
	            Block &&block_false,
	            Scope &parent_expr);

	DEFINE_IMPLEMENT_WITH_MEMBERS(*condition_, block_true_, block_false_)

	const BooleanExpression &condition() const;
	const Block &block_true() const;
	const Block &block_false() const;

protected:
	unique_ptr<BooleanExpression> condition_;
	Block block_true_;
	Block block_false_;
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

private:
	shared_ptr<AbstractVariable> variable_;
	Block block_;
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
	While(unique_ptr<BooleanExpression> &&expression, unique_ptr<Block> &&block, Scope &parent_scope);
	DEFINE_IMPLEMENT_WITH_MEMBERS(*expression_, *block_)

	const BooleanExpression &expression() const;
	const Block &block() const;

protected:
	unique_ptr<BooleanExpression> expression_;
	unique_ptr<Block> block_;
};


class Procedure : public Statement, public Identifier, public LanguageElement<Procedure> {
public:
    Procedure(const string &name, const vector<string> &arg_names, unique_ptr<Block> &&block);
    Procedure(Procedure &&) = default;
	DEFINE_IMPLEMENT_WITH_MEMBERS(scope_, *block_)

	const Scope &scope() const;
	const Block &block() const;
	const vector<string> &args() const;
	shared_ptr<AbstractVariable> argument(arity_t idx) const;

	template<class ExpressionT>
	void declare_variable(const string &name)
	{
		if (std::find(args_.begin(), args_.end(), name) == args_.end())
			throw Bug("Variable `" + name + "' not defined for Procedure `" + this->name() + "'");
		scope_.variable<ExpressionT>(name);
	}

protected:
    Scope scope_;
    unique_ptr<Block> block_;
    vector<string> args_;
};


} // namespace gologpp


#endif // PROCEDURE_H
