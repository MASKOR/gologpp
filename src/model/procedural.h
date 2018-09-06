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


class Block : public Statement, public ScopeOwner, public LanguageElement<Block> {
public:
	Block(Scope *own_scope, const vector<Statement *> &elements);
	virtual void implement(Implementor &) override;

	const vector<unique_ptr<Statement>> &elements() const;

	virtual string to_string(const string &pfx) const override;

private:
	vector<unique_ptr<Statement>> elements_;
};


class Choose : public Statement, public ScopeOwner, public LanguageElement<Choose> {
public:
	Choose(Scope *own_scope, const vector<Statement *> &alternatives);
	void implement(Implementor &) override;

	const vector<unique_ptr<Statement>> &alternatives() const;

	virtual string to_string(const string &pfx) const override;

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

	virtual string to_string(const string &pfx) const override;

protected:
	unique_ptr<BooleanExpression> condition_;
	unique_ptr<Statement> block_true_;
	unique_ptr<Statement> block_false_;
};


template<class LhsT>
class Assignment : public Statement, public LanguageElement<Assignment<LhsT>> {
public:
	static_assert(!std::is_base_of<Statement, LhsT>::value, "Cannot assign to a Statement");

	Assignment(Reference<LhsT> *lhs, typename LhsT::expression_t *rhs, Scope &parent_scope)
	: Statement(parent_scope)
	, lhs_(lhs)
	, rhs_(rhs)
	{}

	DEFINE_IMPLEMENT_WITH_MEMBERS(*lhs_, *rhs_)

	const Reference<LhsT> &lhs() const
	{ return *lhs_; }

	const typename LhsT::expression_t &rhs() const
	{ return *rhs_; }

	virtual string to_string(const string &pfx) const override
	{ return lhs().to_string(pfx) + " = " + rhs().to_string(""); }

private:
    unique_ptr<Reference<LhsT>> lhs_;
    unique_ptr<typename LhsT::expression_t> rhs_;
};


class Pick : public Statement, public ScopeOwner, public LanguageElement<Pick> {
public:
	Pick(Scope *own_scope, const shared_ptr<AbstractVariable> &variable, Statement *stmt);
	Pick(const string &var_name, Statement *stmt, Scope &parent_scope);
	DEFINE_IMPLEMENT_WITH_MEMBERS(scope(), *variable_, *statement_)

	const AbstractVariable &variable() const;
	const Statement &statement() const;

	virtual string to_string(const string &pfx) const override;

private:
	shared_ptr<AbstractVariable> variable_;
	unique_ptr<Statement> statement_;
};


class Search : public Statement, public LanguageElement<Search> {
public:
	Search(Statement *statement, Scope &parent_scope);
	DEFINE_IMPLEMENT_WITH_MEMBERS(*statement_)

	const Statement &statement() const;

	virtual string to_string(const string &pfx) const override;

private:
	unique_ptr<Statement> statement_;
};


class Test : public Statement, public LanguageElement<Test> {
public:
	Test(BooleanExpression *expression, Scope &parent_scope);
	DEFINE_IMPLEMENT_WITH_MEMBERS(*expression_)

	const BooleanExpression &expression() const;

	virtual string to_string(const string &pfx) const override;

protected:
	unique_ptr<BooleanExpression> expression_;
};


class While : public Statement, public LanguageElement<While> {
public:
	While(BooleanExpression *expression, Statement *stmt, Scope &parent_scope);
	DEFINE_IMPLEMENT_WITH_MEMBERS(*expression_, *statement_)

	const BooleanExpression &expression() const;
	const Statement &statement() const;

	virtual string to_string(const string &pfx) const override;

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

	virtual string to_string(const string &pfx) const override
	{ return linesep + pfx + "return " + expression().to_string(""); }

private:
	unique_ptr<ExpressionT> expr_;
};


class AbstractFunction
: public Global
, public ScopeOwner
, public virtual AbstractLanguageElement
{
public:
	typedef Expression expression_t;

	AbstractFunction(Scope *own_scope, const string &name, const vector<shared_ptr<AbstractVariable>> &args, Statement *definition);

	virtual ~AbstractFunction() override;
	virtual ExpressionTypeTag expression_type_tag() const = 0;

	const Statement &definition() const;
	void define(boost::optional<Statement *> definition);
	virtual void compile(AExecutionContext &ctx) override;

protected:
	unique_ptr<Statement> definition_;
	vector<shared_ptr<AbstractVariable>> args_;
};


template<class ExpressionT>
class Function
: public AbstractFunction
, public LanguageElement<Function<ExpressionT>>
{
public:
	typedef AbstractFunction abstract_t;
	typedef ExpressionT expression_t;

	Function(
		Scope *own_scope,
		const string &name,
		const vector<shared_ptr<AbstractVariable>> &args,
		boost::optional<Statement *> statement
	)
	: AbstractFunction(own_scope, name, args, statement.get_value_or(nullptr))
	{}

	Function(
		Scope *own_scope,
		const string &name,
		const vector<shared_ptr<AbstractVariable>> &args
	)
	: Function(own_scope, name, args, boost::optional<Statement *>())
	{}


	Function(Function &&) = default;
	DEFINE_IMPLEMENT_WITH_MEMBERS(scope(), *definition_)

	virtual Expression *ref(Scope &parent_scope, const vector<Expression *> &args) override
	{ return make_reference<Function<ExpressionT>>(parent_scope, args); }

	virtual ExpressionTypeTag expression_type_tag() const override
	{ return ExpressionT::static_type_tag(); }


	virtual string to_string(const string &pfx) const override
	{
		string fn;
		if (expression_type_tag() == ExpressionTypeTag::STATEMENT)
			fn = "procedure ";
		else
			fn = gologpp::to_string(expression_type_tag()) + "function ";

		return linesep + pfx + fn + name() + '(' + concat_list(args(), ", ") + ") " + definition().to_string(pfx);
	}
};


} // namespace gologpp


#endif // PROCEDURE_H
