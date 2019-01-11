#ifndef PROCEDURE_H
#define PROCEDURE_H

#include <vector>
#include <memory>
#include <algorithm>

#include "gologpp.h"

#include "language.h"
#include "expressions.h"
#include "error.h"
#include "global.h"
#include "scope.h"

namespace gologpp {



class Block : public VoidExpression, public ScopeOwner, public LanguageElement<Block> {
public:
	Block(Scope *own_scope, const vector<VoidExpression *> &elements);
	virtual void attach_semantics(SemanticsFactory &) override;

	const vector<unique_ptr<VoidExpression>> &elements() const;

	virtual string to_string(const string &pfx) const override;

private:
	vector<unique_ptr<VoidExpression>> elements_;
};



class Choose : public VoidExpression, public ScopeOwner, public LanguageElement<Choose> {
public:
	Choose(Scope *own_scope, const vector<VoidExpression *> &alternatives);
	void attach_semantics(SemanticsFactory &) override;

	const vector<unique_ptr<VoidExpression>> &alternatives() const;

	virtual string to_string(const string &pfx) const override;

private:
	vector<unique_ptr<VoidExpression>> alternatives_;
};



class Conditional : public VoidExpression, public NoScopeOwner, public LanguageElement<Conditional> {
public:
	Conditional(
		BooleanExpression *condition,
		VoidExpression *block_true,
		const boost::optional<VoidExpression *> &block_false
	);

	DEFINE_IMPLEMENT_WITH_MEMBERS(*condition_, *block_true_, *block_false_)

	const BooleanExpression &condition() const;
	const VoidExpression &block_true() const;
	const VoidExpression &block_false() const;

	virtual string to_string(const string &pfx) const override;

protected:
	unique_ptr<BooleanExpression> condition_;
	unique_ptr<VoidExpression> block_true_;
	unique_ptr<VoidExpression> block_false_;
};



/**
 * @brief Execute a set of statements in parallel.
 */
class Concurrent : public VoidExpression, public ScopeOwner, public LanguageElement<Concurrent> {
public:
	Concurrent(Scope *own_scope, const vector<VoidExpression *> &procs);
	void attach_semantics(SemanticsFactory &) override;

	const vector<unique_ptr<VoidExpression>> &procs() const;

	virtual string to_string(const string &pfx) const override;

private:
	vector<unique_ptr<VoidExpression>> procs_;
};



template<class LhsT>
class Assignment : public VoidExpression, public NoScopeOwner, public LanguageElement<Assignment<LhsT>> {
public:
	static_assert(!std::is_base_of<VoidExpression, LhsT>::value, "Cannot assign to a Statement");

	Assignment(Reference<LhsT> *lhs, typename LhsT::expression_t *rhs)
	: lhs_(lhs), rhs_(rhs)
	{
		lhs_->set_parent(this);
		rhs_->set_parent(this);
	}

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



template<class ExprT>
class Pick : public VoidExpression, public ScopeOwner, public LanguageElement<Pick<ExprT>> {
public:
	Pick(
		Scope *own_scope,
		const shared_ptr<Variable<ExprT>> &variable,
		const boost::optional<std::vector<Constant<ExprT> *>> &domain,
		VoidExpression *statement
	)
	: ScopeOwner(own_scope)
	, variable_(variable)
	, statement_(statement)
	{
		if (domain)
			for (Constant<ExprT> *c : *domain) {
				c->set_parent(this);
				domain_.emplace_back(c);
			}
		dynamic_cast<Expression *>(variable_.get())->set_parent(this);
		statement_->set_parent(this);
	}


	const vector<unique_ptr<Constant<ExprT>>> &domain() const
	{ return domain_; }

	const Variable<ExprT> &variable() const
	{ return *variable_; }

	const VoidExpression &statement() const
	{ return *statement_; }


	virtual void attach_semantics(SemanticsFactory &f) override
	{
		if (semantics_)
			return;

		for (unique_ptr<Constant<ExprT>> & c : domain_)
			c->attach_semantics(f);
		variable_->attach_semantics(f);
		statement_->attach_semantics(f);

		set_implementation(f.make_semantics(*this));
	}


	virtual string to_string(const string &pfx) const override
	{ return linesep + pfx + "pick (" + variable().to_string("") + "): " + statement().to_string(pfx); }

private:
	vector<unique_ptr<Constant<ExprT>>> domain_;
	shared_ptr<Variable<ExprT>> variable_;
	unique_ptr<VoidExpression> statement_;
};



class Search : public VoidExpression, public NoScopeOwner, public LanguageElement<Search> {
public:
	Search(VoidExpression *statement);
	DEFINE_IMPLEMENT_WITH_MEMBERS(*statement_)

	const VoidExpression &statement() const;

	virtual string to_string(const string &pfx) const override;

protected:
	unique_ptr<VoidExpression> statement_;
};


class Solve : public Search, public LanguageElement<Solve> {
public:
	Solve(
		NumericExpression *horizon,
		Reference<NumericFunction> *reward,
		VoidExpression *statement
	);

	const NumericExpression &horizon() const;
	const Reference<NumericFunction> &reward() const;
	virtual void attach_semantics(SemanticsFactory &implementor) override;
	virtual string to_string(const string &pfx) const override;

private:
	unique_ptr<NumericExpression> horizon_;
	unique_ptr<Reference<NumericFunction>> reward_;
};


class Test : public VoidExpression, public NoScopeOwner, public LanguageElement<Test> {
public:
	Test(BooleanExpression *expression);
	DEFINE_IMPLEMENT_WITH_MEMBERS(*expression_)

	const BooleanExpression &expression() const;

	virtual string to_string(const string &pfx) const override;

protected:
	unique_ptr<BooleanExpression> expression_;
};


class While : public VoidExpression, public NoScopeOwner, public LanguageElement<While> {
public:
	While(BooleanExpression *expression, VoidExpression *stmt);
	DEFINE_IMPLEMENT_WITH_MEMBERS(*expression_, *statement_)

	const BooleanExpression &expression() const;
	const VoidExpression &statement() const;

	virtual string to_string(const string &pfx) const override;

protected:
	unique_ptr<BooleanExpression> expression_;
	unique_ptr<VoidExpression> statement_;
};


template<class ExpressionT>
class Return : public VoidExpression, public NoScopeOwner, public LanguageElement<Return<ExpressionT>> {
public:
	Return(ExpressionT *expr)
	: expr_(expr)
	{ expr_->set_parent(this); }

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

	AbstractFunction(Scope *own_scope, const string &name, const vector<shared_ptr<AbstractVariable>> &args);

	virtual ~AbstractFunction() override;
	virtual ExpressionTypeTag expression_type_tag() const = 0;

	const VoidExpression &definition() const;
	void define(VoidExpression *definition);
	virtual void compile(AExecutionContext &ctx) override;

protected:
	unique_ptr<VoidExpression> definition_;
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
		const vector<shared_ptr<AbstractVariable>> &args
	)
	: AbstractFunction(own_scope, name, args)
	{}

	Function(
		Scope *own_scope,
		const string &name,
		const boost::optional<vector<shared_ptr<AbstractVariable>>> &args
	)
	: AbstractFunction(own_scope, name, args.get_value_or({}))
	{}

	Function(Function &&) = default;
	DEFINE_IMPLEMENT_WITH_MEMBERS(scope(), *definition_)

	Reference<Function<ExpressionT>> *make_ref(const vector<Expression *> &args)
	{ return make_ref_<Function<ExpressionT>>(args); }

	virtual Expression *ref(const vector<Expression *> &args) override
	{ return make_ref(args); }

	virtual ExpressionTypeTag expression_type_tag() const override
	{ return ExpressionT::static_type_tag(); }


	virtual string to_string(const string &pfx) const override
	{
		string fn;
		if (expression_type_tag() == ExpressionTypeTag::VOID)
			fn = "procedure ";
		else
			fn = gologpp::to_string(expression_type_tag()) + "function ";

		return linesep + pfx + fn + name() + '('
			+ concat_list(args(), ", ")
			+ ") " + definition().to_string(pfx);
	}
};


} // namespace gologpp


#endif // PROCEDURE_H
