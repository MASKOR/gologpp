/*************************************************************************
 * This file is part of golog++.
 *
 * golog++ is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * golog++ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with golog++.  If not, see <https://www.gnu.org/licenses/>.
**************************************************************************/

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
#include "mapping.h"

namespace gologpp {

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief A scoped block of procedural code.
 */
class Block : public Instruction, public ScopeOwner, public LanguageElement<Block, VoidType> {
public:
	Block(Scope *own_scope, const vector<Instruction *> &elements);
	virtual void attach_semantics(SemanticsFactory &) override;

	const vector<unique_ptr<Instruction>> &elements() const;

	virtual string to_string(const string &pfx) const override;

private:
	vector<unique_ptr<Instruction>> elements_;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Nondeterministic choice from a set of @ref Statement.
 */
class Choose : public Instruction, public ScopeOwner, public LanguageElement<Choose, VoidType> {
public:
	Choose(Scope *own_scope, const vector<Instruction *> &alternatives);
	void attach_semantics(SemanticsFactory &) override;

	const vector<unique_ptr<Instruction>> &alternatives() const;

	virtual string to_string(const string &pfx) const override;

private:
	vector<unique_ptr<Instruction>> alternatives_;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Classical if-then-else.
 */
template<class SignT>
class Conditional
: public SignT
, public NoScopeOwner
, public LanguageElement <
	Conditional<SignT>,
	typename std::conditional<
		std::is_same<SignT, Instruction>::value,
		VoidType,
		UndefinedType
	>::type
> {
public:
	Conditional(
		Expression *condition,
		SignT *block_true,
		SignT *block_false
	);

	DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(*condition_, *block_true_, *block_false_)

	const Expression &condition() const;
	const SignT &block_true() const;
	const SignT &block_false() const;

	virtual string to_string(const string &pfx) const override;

protected:
	SafeExprOwner<BoolType> condition_;
	unique_ptr<SignT> block_true_;
	unique_ptr<SignT> block_false_;
};

extern template class Conditional<Instruction>;
extern template class Conditional<Expression>;

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Execute a set of statements in parallel.
 */
class Concurrent : public Instruction, public ScopeOwner, public LanguageElement<Concurrent, VoidType> {
public:
	Concurrent(Scope *own_scope, const vector<Instruction *> &procs);
	void attach_semantics(SemanticsFactory &) override;

	const vector<unique_ptr<Instruction>> &procs() const;

	virtual string to_string(const string &pfx) const override;

private:
	vector<unique_ptr<Instruction>> procs_;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

class AbstractAssignment
: public Instruction
, public NoScopeOwner
, public virtual ModelElement {
public:
	virtual const Expression &lhs() const = 0;
	virtual const Expression &rhs() const = 0;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

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
, public LanguageElement<Assignment<LhsT>, VoidType> {
public:
	static_assert(!std::is_base_of<Function, LhsT>::value, "Cannot assign to a function");

	Assignment(LhsT *lhs, Expression *rhs)
	: lhs_(lhs), rhs_(rhs)
	{
		if (lhs_->type().template is<VoidType>())
			throw TypeError("Cannot assign to a void expression");

		if (!(lhs_->type() >= *rhs))
			throw TypeError(*rhs, lhs_->type());

		lhs_->set_parent(this);
		rhs_->set_parent(this);
	}

	DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(*lhs_, *rhs_)

	const LhsT &lhs() const override
	{ return *lhs_; }

	const Expression &rhs() const override
	{ return *rhs_; }

	virtual string to_string(const string &pfx) const override
	{ return lhs().to_string(pfx) + " = " + rhs().to_string("") + ';'; }

private:
    unique_ptr<LhsT> lhs_;
    unique_ptr<Expression> rhs_;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class Pick
 *
 * @brief Nondeterministically pick a variable assignment.
 */
class Pick : public Instruction, public ScopeOwner, public LanguageElement<Pick, VoidType> {
public:
	Pick(
		Scope *own_scope,
		const shared_ptr<Variable> &variable,
		const boost::optional<std::vector<Value *>> &domain,
		Instruction *statement
	);

	const vector<unique_ptr<Value>> &domain() const;
	const Variable &variable() const;
	const Instruction &statement() const;

	virtual void attach_semantics(SemanticsFactory &f) override;

	virtual string to_string(const string &pfx) const override;

private:
	vector<unique_ptr<Value>> domain_;
	shared_ptr<Variable> variable_;
	unique_ptr<Instruction> statement_;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Plan and execute.
 *
 * Resolve all nondeterinisms within a statement so that all its tests succeed and all its actions
 * become executable.
 */
class Search : public Instruction, public NoScopeOwner, public LanguageElement<Search, VoidType> {
public:
	Search(Instruction *statement);
	DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(*statement_)

	const Instruction &statement() const;

	virtual string to_string(const string &pfx) const override;

protected:
	unique_ptr<Instruction> statement_;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Plan with a reward function up to a search horizon, then execute.
 *
 * Search for a "best" executable path given a reward function, but only up to a
 * certain maximum number of actions (the horizon). Then execute the found action sequence.
 */
class Solve : public Instruction, public NoScopeOwner, public LanguageElement<Solve, VoidType> {
public:
	Solve(
		Expression *horizon,
		Reference<Function> *reward,
		Instruction *statement
	);

	const Instruction &statement() const;
	const Expression &horizon() const;
	const Reference<Function> &reward() const;
	virtual void attach_semantics(SemanticsFactory &implementor) override;
	virtual string to_string(const string &pfx) const override;

private:
	unique_ptr<Instruction> statement_;
	SafeExprOwner<NumberType> horizon_;
	unique_ptr<Reference<Function>> reward_;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Test for a boolean condition. Fail the program if the condition evaluates to false.
 */
class Test : public Instruction, public NoScopeOwner, public LanguageElement<Test, VoidType> {
public:
	Test(Expression *expression);
	DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(*expression_)

	const Expression &expression() const;

	virtual string to_string(const string &pfx) const override;

protected:
	SafeExprOwner<BoolType> expression_;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Classical while loop.
 */
class While : public Instruction, public NoScopeOwner, public LanguageElement<While, VoidType> {
public:
	While(Expression *expression, Instruction *stmt);
	DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(*expression_, *statement_)

	const Expression &expression() const;
	const Instruction &statement() const;

	virtual string to_string(const string &pfx) const override;

protected:
	SafeExprOwner<BoolType> expression_;
	unique_ptr<Instruction> statement_;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

class Procedure
: public ScopeOwner
, public Signified<Instruction>
, public LanguageElement<Procedure, VoidType>
{
public:
	Procedure(
		Scope *own_scope,
		const Type &t,
		const string &name,
		const vector<shared_ptr<Variable>> &params
	);

	Procedure(
		Scope *own_scope,
		const Type &t,
		const string &name,
		const boost::optional<vector<shared_ptr<Variable>>> &params
	);


	Reference<Procedure> *make_ref(const vector<Expression *> &params);
	virtual Instruction *ref(const vector<Expression *> &params) override;
	virtual const Instruction &definition() const;
	void define(Instruction *definition);
	virtual void compile(AExecutionController &ctx) override;

	virtual string to_string(const string &pfx) const override;

	DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(scope(), *definition_)

private:
	unique_ptr<Instruction> definition_;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

class Function
: public ScopeOwner
, public Signified<Expression>
, public LanguageElement<Function>
{
public:
	Function(
		Scope *own_scope,
		const Type &t,
		const string &name,
		const vector<shared_ptr<Variable>> &params
	);

	Function(
		Scope *own_scope,
		const Type &t,
		const string &name,
		const boost::optional<vector<shared_ptr<Variable>>> &params
	);


	virtual Expression *ref(const vector<Expression *> &params) override;
	Reference<Function> *make_ref(const vector<Expression *> &params);
	virtual const Expression &definition() const;
	void define(Expression *definition);
	virtual void compile(AExecutionController &ctx) override;

	virtual string to_string(const string &pfx) const override;

	DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(scope(), *definition_)

private:
	unique_ptr<Expression> definition_;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

class ExogFunction
: public ScopeOwner
, public Signified<Expression>
, public LanguageElement<ExogFunction>
{
public:
	ExogFunction(
		Scope *own_scope,
		const Type &t,
		const string &name,
		const vector<shared_ptr<Variable>> &params
	);

	ExogFunction(
		Scope *own_scope,
		const Type &t,
		const string &name,
		const boost::optional<vector<shared_ptr<Variable>>> &params
	);

	virtual Expression *ref(const vector<Expression *> &params) override;
	Reference<ExogFunction> *make_ref(const vector<Expression *> &params);
	void define();
	virtual void compile(AExecutionController &ctx) override;

	virtual string to_string(const string &pfx) const override;

	DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(scope())
};



template<>
class GeneralSemantics<Reference<ExogFunction>>
: public virtual GeneralSemantics<Expression>
{
public:
	GeneralSemantics(const Reference<ExogFunction> &elem, AExecutionController &context);

	virtual ~GeneralSemantics<Reference<ExogFunction>>() = default;

	virtual Value evaluate(const BindingChain &, const History &) override;

	const Reference<ExogFunction> &element() const;
	virtual AExecutionController &context() const override;
	void update_element(const Reference<ExogFunction> *new_element);
	virtual const ModelElement &model_element() const override;
	virtual const Expression &expression() const override;

private:
	const Reference<ExogFunction> *element_;
	AExecutionController &context_;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

class DurativeCall
: public Instruction
, public NoScopeOwner
, public LanguageElement<DurativeCall, VoidType>
{
public:
	enum class Hook {
		FINISH = 0, FAIL,
		END, // So we have FINISH < END && FAIL < END

		START, CANCEL,
	};

	DurativeCall(Hook hook, Reference<Action> *action);
	DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(*action_)

	Hook hook() const;
	const Reference<Action> &action() const;
	virtual string to_string(const string &pfx) const override;

private:
	const Hook hook_;
	const unique_ptr<Reference<Action>> action_;
};


string to_string(DurativeCall::Hook);

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

class FieldAccess
: public Expression
, public NoScopeOwner
, public LanguageElement<FieldAccess>
{
public:
	FieldAccess(Expression *subject, const string &field_name);
	const Expression &subject() const;
	const string &field_name() const;

	DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(*subject_)

	virtual const Type &type() const override;

	string to_string(const string &pfx) const override;

private:
	SafeExprOwner<CompoundType> subject_;
	const string field_name_;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

class ListAccess
: public Expression
, public NoScopeOwner
, public LanguageElement<ListAccess>
{
public:
	ListAccess(Expression *subject, Expression *index);
	const Expression &subject() const;
	const Expression &index() const;

	DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(*subject_, *index_)

	virtual const Type &type() const override;

	string to_string(const string &pfx) const override;

private:
	SafeExprOwner<ListType> subject_;
	SafeExprOwner<NumberType> index_;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

class ListLength
: public Expression
, public NoScopeOwner
, public LanguageElement<ListLength, NumberType>
{
public:
	ListLength(Expression *subject);
	const Expression &subject() const;

	DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(*subject_)

	string to_string(const string &pfx) const override;

private:
	SafeExprOwner<ListType> subject_;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

enum ListOpEnd {
	FRONT, BACK
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

class ListPop
: public Instruction
, public NoScopeOwner
, public LanguageElement<ListPop, VoidType>
{
public:
	ListPop(Expression *list, ListOpEnd which_end);
	const Expression &list() const;
	ListOpEnd which_end() const;

	DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(*list_)

	string to_string(const string &pfx) const override;

private:
	SafeExprOwner<ListType> list_;
	ListOpEnd which_end_;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

class ListPush
: public Instruction
, public NoScopeOwner
, public LanguageElement<ListPush, VoidType>
{
public:
	ListPush(Expression *list, ListOpEnd which_end, Expression *what);
	const Expression &list() const;
	ListOpEnd which_end() const;
	const Expression &what() const;

	DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(*list_, *what_)

	string to_string(const string &pfx) const override;

private:
	SafeExprOwner<ListType> list_;
	ListOpEnd which_end_;
	unique_ptr<Expression> what_;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

class During
: public Instruction
, public NoScopeOwner
, public LanguageElement<During, VoidType>
{
public:
	During(
		Reference<Action> *action_call,
		Instruction *parallel_block,
		boost::optional<Instruction *> on_fail,
		boost::optional<Instruction *> on_cancel
	);
	const Reference<Action> &action_call() const;
	const Instruction &parallel_block() const;
	const Instruction &on_fail() const;
	const Instruction &on_cancel() const;

	DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(*action_call_, *parallel_block_, *on_fail_, *on_cancel_)

	string to_string(const string &pfx) const override;

private:
	unique_ptr<Reference<Action>> action_call_;
	unique_ptr<Instruction> parallel_block_;
	unique_ptr<Instruction> on_fail_;
	unique_ptr<Instruction> on_cancel_;
};



} // namespace gologpp



#endif // PROCEDURE_H


