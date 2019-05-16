/**
 * @file procedural.h Header for all classes that represent procedural code.
 */

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
#include "action.h"
#include "reference.h"
#include "fluent.h"

namespace gologpp {


/**
 * @brief A scoped block of procedural code.
 */
class Block : public VoidExpression, public ScopeOwner, public LanguageElement<Block> {
public:
	Block(Scope *own_scope, const vector<Expression *> &elements);
	virtual void attach_semantics(SemanticsFactory &) override;

	const vector<unique_ptr<VoidExpression>> &elements() const;

	virtual string to_string(const string &pfx) const override;

private:
	vector<unique_ptr<VoidExpression>> elements_;
};



/**
 * @brief Nondeterministic choice from a set of @ref Statement.
 */
class Choose : public VoidExpression, public ScopeOwner, public LanguageElement<Choose> {
public:
	Choose(Scope *own_scope, const vector<Expression *> &alternatives);
	void attach_semantics(SemanticsFactory &) override;

	const vector<unique_ptr<VoidExpression>> &alternatives() const;

	virtual string to_string(const string &pfx) const override;

private:
	vector<unique_ptr<VoidExpression>> alternatives_;
};



/**
 * @brief Classical if-then-else.
 */
class Conditional : public VoidExpression, public NoScopeOwner, public LanguageElement<Conditional> {
public:
	Conditional(
		Expression *condition,
		Expression *block_true,
		Expression *block_false
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
	Concurrent(Scope *own_scope, const vector<Expression *> &procs);
	void attach_semantics(SemanticsFactory &) override;

	const vector<unique_ptr<VoidExpression>> &procs() const;

	virtual string to_string(const string &pfx) const override;

private:
	vector<unique_ptr<VoidExpression>> procs_;
};



class AbstractAssignment
: public VoidExpression
, public NoScopeOwner
, public virtual AbstractLanguageElement {
public:
	virtual const Expression &lhs() const = 0;
	virtual const Expression &rhs() const = 0;
};



/**
 * @class Assignment
 * @tparam LhsT The type of the left hand side expression.
 * @brief Type-safe assignment.
 *
 * The type of the right hand side must have the same Expression type as the left hand side.
 * @see Assignment::Assignment.
 */
template<class LhsT>
class Assignment
: public AbstractAssignment
, public LanguageElement<Assignment<LhsT>> {
public:
	static_assert(!std::is_base_of<Expression, LhsT>::value, "Cannot assign to a statement");
	static_assert(!std::is_base_of<AbstractFunction, LhsT>::value, "Cannot assign to a function");

	Assignment(LhsT *lhs, Expression *rhs)
	: lhs_(lhs), rhs_(rhs)
	{
		ensure_type_equality(*lhs, *rhs);
		lhs_->set_parent(this);
		rhs_->set_parent(this);
	}

	DEFINE_IMPLEMENT_WITH_MEMBERS(*lhs_, *rhs_)

	const LhsT &lhs() const override
	{ return *lhs_; }

	const Expression &rhs() const override
	{ return *rhs_; }

	virtual string to_string(const string &pfx) const override
	{ return lhs().to_string(pfx) + " = " + rhs().to_string(""); }

private:
    unique_ptr<LhsT> lhs_;
    unique_ptr<Expression> rhs_;
};



/**
 * @class Pick
 *
 * @brief Nondeterministically pick a variable assignment.
 */
class Pick : public VoidExpression, public ScopeOwner, public LanguageElement<Pick> {
public:
	Pick(
		Scope *own_scope,
		const shared_ptr<Variable> &variable,
		const boost::optional<std::vector<Constant *>> &domain,
		Expression *statement
	)
	: ScopeOwner(own_scope)
	, variable_(variable)
	, statement_(statement)
	{
		if (domain)
			for (Constant *c : *domain) {
				ensure_type_equality(*variable, *c);
				c->set_parent(this);
				domain_.emplace_back(c);
			}
		dynamic_cast<Expression *>(variable_.get())->set_parent(this);
		statement_->set_parent(this);
	}


	const vector<unique_ptr<Constant>> &domain() const
	{ return domain_; }

	const Variable &variable() const
	{ return *variable_; }

	const Expression &statement() const
	{ return *statement_; }


	virtual void attach_semantics(SemanticsFactory &f) override
	{
		if (semantics_)
			return;

		for (unique_ptr<Constant> & c : domain_)
			c->attach_semantics(f);
		variable_->attach_semantics(f);
		statement_->attach_semantics(f);

		set_implementation(f.make_semantics(*this));
	}


	virtual string to_string(const string &pfx) const override
	{ return linesep + pfx + "pick (" + variable().to_string("") + "): " + statement().to_string(pfx); }

private:
	vector<unique_ptr<Constant>> domain_;
	shared_ptr<Variable> variable_;
	unique_ptr<VoidExpression> statement_;
};



/**
 * @brief Plan and execute.
 *
 * Resolve all nondeterinisms within a statement so that all its tests succeed and all its actions
 * become executable.
 */
class Search : public VoidExpression, public NoScopeOwner, public LanguageElement<Search> {
public:
	Search(Expression *statement);
	DEFINE_IMPLEMENT_WITH_MEMBERS(*statement_)

	const VoidExpression &statement() const;

	virtual string to_string(const string &pfx) const override;

protected:
	unique_ptr<VoidExpression> statement_;
};



/**
 * @brief Plan with a reward function up to a search horizon, then execute.
 *
 * Search for a "best" executable path given a reward function, but only up to a
 * certain maximum number of actions (the horizon). Then execute the found action sequence.
 */
class Solve : public VoidExpression, public NoScopeOwner, public LanguageElement<Solve> {
public:
	Solve(
		Expression *horizon,
		Reference<Function> *reward,
		Expression *statement
	);

	const VoidExpression &statement() const;
	const NumericExpression &horizon() const;
	const Reference<Function> &reward() const;
	virtual void attach_semantics(SemanticsFactory &implementor) override;
	virtual string to_string(const string &pfx) const override;

private:
	unique_ptr<VoidExpression> statement_;
	unique_ptr<NumericExpression> horizon_;
	unique_ptr<Reference<Function>> reward_;
};



/**
 * @brief Test for a boolean condition. Fail the program if the condition evaluates to false.
 */
class Test : public VoidExpression, public NoScopeOwner, public LanguageElement<Test> {
public:
	Test(Expression *expression);
	DEFINE_IMPLEMENT_WITH_MEMBERS(*expression_)

	const BooleanExpression &expression() const;

	virtual string to_string(const string &pfx) const override;

protected:
	unique_ptr<BooleanExpression> expression_;
};



/**
 * @brief Classical while loop.
 */
class While : public VoidExpression, public NoScopeOwner, public LanguageElement<While> {
public:
	While(Expression *expression, Expression *stmt);
	DEFINE_IMPLEMENT_WITH_MEMBERS(*expression_, *statement_)

	const BooleanExpression &expression() const;
	const VoidExpression &statement() const;

	virtual string to_string(const string &pfx) const override;

protected:
	unique_ptr<BooleanExpression> expression_;
	unique_ptr<VoidExpression> statement_;
};



/**
 * @brief Return a value from a function.
 */
class Return : public VoidExpression, public NoScopeOwner, public LanguageElement<Return> {
public:
	Return(Expression *expr)
	: expr_(expr)
	{ expr_->set_parent(this); }

	DEFINE_IMPLEMENT_WITH_MEMBERS(*expr_)

	const Expression &expression() const
	{ return *expr_; }

	virtual string to_string(const string &pfx) const override
	{ return linesep + pfx + "return " + expression().to_string(""); }

private:
	unique_ptr<Expression> expr_;
};



/**
 * @brief The abstract superclass of all @ref Function types.
 */
class AbstractFunction
: public Global
, public ScopeOwner
, public virtual AbstractLanguageElement
{
protected:
	AbstractFunction(
		Scope *own_scope,
		const string &type_name,
		const string &name,
		const vector<shared_ptr<Variable>> &args
	);

public:
	typedef AbstractFunction abstract_t;

	virtual ~AbstractFunction() override;

	const VoidExpression &definition() const;
	void define(Expression *definition);
	virtual void compile(AExecutionContext &ctx) override;

protected:
	unique_ptr<VoidExpression> definition_;
	vector<shared_ptr<Variable>> args_;
};



/**
 * @brief A function, also called a subroutine.
 * A function that returns a @ref Expression is also called a @ref Procedure.
 */
class Function
: public AbstractFunction
, public LanguageElement<Function>
{
public:
	Function(
		Scope *own_scope,
		const string &type_name,
		const string &name,
		const vector<shared_ptr<Variable>> &args
	)
	: AbstractFunction(own_scope, type_name, name, args)
	{}

	Function(
		Scope *own_scope,
		const string &type_name,
		const string &name,
		const boost::optional<vector<shared_ptr<Variable>>> &args
	)
	: AbstractFunction(own_scope, type_name, name, args.get_value_or({}))
	{}

	DEFINE_IMPLEMENT_WITH_MEMBERS(scope(), *definition_)

	Reference<Function> *make_ref(const vector<Expression *> &args)
	{ return make_ref_<Function>(args); }

	virtual Expression *ref(const vector<Expression *> &args) override
	{ return make_ref(args); }


	virtual string to_string(const string &pfx) const override
	{
		string fn;
		if (type().dynamic_tag() == ExpressionTypeTag::VOID)
			fn = "procedure ";
		else
			fn = gologpp::to_string(type().dynamic_tag()) + "function ";

		return linesep + pfx + fn + name() + '('
			+ concat_list(args(), ", ")
			+ ") " + definition().to_string(pfx);
	}
};



class DurativeCall
: public VoidExpression
, public NoScopeOwner
, public LanguageElement<DurativeCall>
{
public:
	enum Hook {
		START, FINISH, FAIL, STOP
	};

	DurativeCall(Hook hook, Reference<Action> *action);
	DEFINE_IMPLEMENT_WITH_MEMBERS(*action_)

	Hook hook() const;
	const Reference<Action> &action() const;
	virtual string to_string(const string &pfx) const override;

private:
	const Hook hook_;
	const unique_ptr<Reference<Action>> action_;
};


string to_string(DurativeCall::Hook);



class AbstractFieldAccess
: public Expression
, public NoScopeOwner
, public virtual AbstractLanguageElement
{
public:
	AbstractFieldAccess(Expression *subject, const string &field_name);
	const CompoundExpression &subject() const;
	const string &field_name() const;

protected:
	unique_ptr<CompoundExpression> subject_;
	const string field_name_;
};


class FieldAccess
: public AbstractFieldAccess
, public LanguageElement<FieldAccess>
{
public:
	using AbstractFieldAccess::AbstractFieldAccess;

	DEFINE_IMPLEMENT_WITH_MEMBERS(*subject_)

	string to_string(const string &pfx) const override
	{ return pfx + subject().str() + "." + gologpp::to_string(type().dynamic_tag()) + field_name(); }
};



FieldAccess *nested_field_access(Expression *subject, const vector<string> &fields);


} // namespace gologpp


#endif // PROCEDURE_H
