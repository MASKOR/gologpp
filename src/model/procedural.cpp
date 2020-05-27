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

#include <functional>
#include <cstdlib>

#include "procedural.h"
#include "formula.h"
#include "execution.h"
#include "reference.h"
#include "expressions.h"

#include "error.h"

namespace gologpp {


Block::Block(Scope *own_scope, const vector<Instruction *> &elements)
: ScopeOwner(own_scope)
{
	for (Instruction *stmt : elements) {
		stmt->set_parent(this);
		elements_.emplace_back(stmt);
	}
}

void Block::attach_semantics(SemanticsFactory &f)
{
	if (!semantics_) {
		semantics_ = f.make_semantics(*this);
		scope().attach_semantics(f);
		for (auto &stmt : elements_)
			stmt->attach_semantics(f);
	}
}

const vector<unique_ptr<Instruction>> &Block::elements() const
{ return elements_; }

string Block::to_string(const string &pfx) const
{
	if (elements().size() == 0)
		return pfx + "{}";
	else if (elements().size() == 1)
		return elements()[0]->to_string(pfx);
	else {
		return pfx + "{" + linesep
			+ concat_list(elements(), linesep, pfx + indent) + linesep
			+ pfx + "}" + linesep;
	}
}



Choose::Choose(Scope *own_scope, const vector<Instruction *> &alternatives)
: ScopeOwner(own_scope)
{
	for (Instruction *stmt : alternatives) {
		stmt->set_parent(this);
		alternatives_.emplace_back(stmt);
	}
}

const vector<unique_ptr<Instruction>> &Choose::alternatives() const
{ return alternatives_; }


void Choose::attach_semantics(SemanticsFactory &f)
{
	if (!semantics_) {
		semantics_ = f.make_semantics(*this);
		scope().attach_semantics(f);
		for (unique_ptr<Instruction> &stmt : alternatives_)
			stmt->attach_semantics(f);
	}
}


string Choose::to_string(const string &pfx) const
{
	return linesep + pfx + "choose {" + linesep
		+ concat_list(alternatives(), linesep, pfx + indent) + linesep
		+ pfx + "}" linesep;
}


template<class SignT>
Conditional<SignT>::Conditional(
	Expression *condition,
	SignT *block_true,
	SignT *block_false
)
: condition_(condition)
, block_true_(block_true)
, block_false_(block_false)
{
	condition_->set_parent(this);
	block_true_->set_parent(this);
	block_false_->set_parent(this);
}

template<class SignT>
const Expression &Conditional<SignT>::condition() const
{ return *condition_; }

template<class SignT>
const SignT &Conditional<SignT>::block_false() const
{ return *block_false_; }

template<class SignT>
const SignT &Conditional<SignT>::block_true() const
{ return *block_true_; }


template<class SignT>
string Conditional<SignT>::to_string(const string &pfx) const
{
	return linesep + pfx + "if (" + condition().to_string("") + ") " + block_true().to_string(pfx)
		+ (block_false_ ? pfx + linesep + "else" + block_false().to_string(pfx) : "");
}

template
class Conditional<Instruction>;

template
class Conditional<Expression>;



Concurrent::Concurrent(Scope *own_scope, const vector<Instruction *> &procs)
: ScopeOwner(own_scope)
{
	for (Instruction *p : procs) {
		p->set_parent(this);
		procs_.emplace_back(p);
	}
}


void Concurrent::attach_semantics(SemanticsFactory &f)
{
	semantics_ = f.make_semantics(*this);
	scope().attach_semantics(f);
	for (unique_ptr<Instruction> &p : procs_)
		p->attach_semantics(f);
}

const vector<unique_ptr<Instruction>> &Concurrent::procs() const
{ return procs_; }


string Concurrent::to_string(const string &pfx) const
{
	return linesep + pfx + "concurrent {" + linesep
		+ concat_list(procs(), linesep, pfx + indent) + linesep
		+ pfx + "}";
}



Pick::Pick(
	Scope *own_scope,
	const shared_ptr<Variable> &variable,
	const boost::optional<std::vector<Value *>> &domain,
	Instruction *statement
)
: ScopeOwner(own_scope)
, variable_(variable)
, statement_(statement)
{
	if (domain)
		for (Value *c : *domain) {
			if (!(variable->type() >= *c))
				throw TypeError(*c, variable->type());

			c->set_parent(this);
			domain_.emplace_back(c);
		}
	dynamic_cast<Expression *>(variable_.get())->set_parent(this);
	statement_->set_parent(this);
}


const vector<unique_ptr<Value>> &Pick::domain() const
{ return domain_; }

const Variable &Pick::variable() const
{ return *variable_; }

const Instruction &Pick::statement() const
{ return *statement_; }

string Pick::to_string(const string &pfx) const
{ return linesep + pfx + "pick (" + variable().to_string("") + "): " + statement().to_string(pfx); }


void Pick::attach_semantics(SemanticsFactory &f)
{
	if (semantics_)
		return;

	for (unique_ptr<Value> & c : domain_)
		c->attach_semantics(f);
	variable_->attach_semantics(f);
	statement_->attach_semantics(f);

	set_semantics(f.make_semantics(*this));
}





Search::Search(Instruction *statement)
: statement_(statement)
{
	statement_->set_parent(this);
}

const Instruction &Search::statement() const
{ return *statement_; }

string Search::to_string(const string &pfx) const
{ return linesep + pfx + "search " + statement().to_string(pfx); }



Solve::Solve(
	Expression *horizon,
	Reference<Function> *reward,
	Instruction *statement
)
: statement_(statement)
, horizon_(horizon)
, reward_(reward)
{
	horizon_->set_parent(this);
	reward_->set_parent(this);
}

const Instruction &Solve::statement() const
{ return *statement_; }

const Expression &Solve::horizon() const
{ return *horizon_; }

const Reference<Function> &Solve::reward() const
{ return *reward_; }


void Solve::attach_semantics(SemanticsFactory &f)
{
	if (semantics_)
		return;
	horizon_->attach_semantics(f);
	reward_->attach_semantics(f);
	statement_->attach_semantics(f);
	semantics_ = f.make_semantics(*this);
}


string Solve::to_string(const string &pfx) const
{
	return linesep + pfx + "solve ("
		+ horizon().to_string(pfx) + ", "
		+ reward().to_string(pfx) + ")"
		+ statement().to_string(pfx + indent);
}



Test::Test(Expression *expression)
: expression_(expression)
{
	expression_->set_parent(this);
}

const Expression &Test::expression() const
{ return *expression_; }

string Test::to_string(const string &pfx) const
{ return linesep + pfx + "test (" + expression().to_string("") + ");"; }



While::While(Expression *expression, Instruction *statement)
: expression_(expression)
, statement_(statement)
{
	expression_->set_parent(this);
	statement_->set_parent(this);
}

const Expression &While::expression() const
{ return *expression_; }

const Instruction &While::statement() const
{ return *statement_; }

string While::to_string(const string &pfx) const
{ return linesep + pfx + "while (" + expression().to_string("") + ") " + statement().to_string(pfx); }



Function::Function(
	Scope *own_scope,
	const Type &t,
	const string &name,
	const vector<shared_ptr<Variable>> &args
)
: ScopeOwner(own_scope)
, Signified<Expression>(name, args)
{ set_type(t); }


Function::Function(
	Scope *own_scope,
	const Type &t,
	const string &name,
	const boost::optional<vector<shared_ptr<Variable>>> &args
)
: ScopeOwner(own_scope)
, Signified<Expression>(name, args.get_value_or({}))
{ set_type(t); }


string Function::to_string(const string &pfx) const
{
	return linesep + pfx + type().name() + " function " + name() + '('
	+ concat_list(params(), ", ")
	+ ") " + definition().to_string(pfx);
}

void Function::define(Expression *definition)
{
	definition_.reset(definition);
	definition_->set_parent(this);
}


void Function::compile(AExecutionContext &ctx)
{ ctx.compile(*this); }

Reference<Function> *Function::make_ref(const vector<Expression *> &args)
{ return make_ref_<Function>(args); }

Expression *Function::ref(const vector<Expression *> &args)
{ return make_ref(args); }

const Expression &Function::definition() const
{ return *definition_; }



Procedure::Procedure(
	Scope *own_scope,
	const Type &t,
	const string &name,
	const vector<shared_ptr<Variable>> &args
)
: ScopeOwner(own_scope)
, Signified<Instruction>(name, args)
{ set_type(t); }


Procedure::Procedure(
	Scope *own_scope,
	const Type &t,
	const string &name,
	const boost::optional<vector<shared_ptr<Variable>>> &args
)
: ScopeOwner(own_scope)
, Signified<Instruction>(name, args.get_value_or({}))
{ set_type(t); }


void Procedure::define(Instruction *definition)
{
	definition_.reset(definition);
	definition_->set_parent(this);
}


string Procedure::to_string(const string &pfx) const
{
	return linesep + pfx + "procedure " + name() + '('
	+ concat_list(params(), ", ")
	+ ") " + definition().to_string(pfx);
}


void Procedure::compile(AExecutionContext &ctx)
{ ctx.compile(*this); }

Reference<Procedure> *Procedure::make_ref(const vector<Expression *> &args)
{ return make_ref_<Procedure>(args); }

Instruction *Procedure::ref(const vector<Expression *> &args)
{ return make_ref(args); }

const Instruction &Procedure::definition() const
{ return *definition_; }





DurativeCall::DurativeCall(DurativeCall::Hook type, Reference<Action> *action)
: hook_(type)
, action_(action)
{
	action_->set_parent(this);
}

DurativeCall::Hook DurativeCall::hook() const
{ return hook_; }

const Reference<Action> &DurativeCall::action() const
{ return *action_; }

string DurativeCall::to_string(const string &pfx) const
{ return linesep + pfx + gologpp::to_string(hook()) + "(" + action().str() + ");"; }



FieldAccess::FieldAccess(Expression *subject, const string &field_name)
: subject_(subject)
, field_name_(field_name)
{
	subject_->set_parent(this);
}

const Expression &FieldAccess::subject() const
{ return *subject_; }

const string &FieldAccess::field_name() const
{ return field_name_; }

const Type &FieldAccess::type() const
{
	return dynamic_cast<const CompoundType &>(
		subject_->type()
	).field_type(field_name_);
}

string FieldAccess::to_string(const string &pfx) const
{ return pfx + subject().str() + "." + field_name(); }



ListAccess::ListAccess(Expression *subject, Expression *index)
: subject_(subject)
, index_(index)
{
	subject_->set_parent(this);
	index_->set_parent(this);
}

const Expression &ListAccess::subject() const
{ return *subject_; }

const Expression &ListAccess::index() const
{ return *index_; }


const Type &ListAccess::type() const
{
	return dynamic_cast<const ListType &>(
		subject_->type()
	).element_type();
}

string ListAccess::to_string(const string &pfx) const
{ return subject_->to_string(pfx) + '[' + index_->str() + ']'; }



ListLength::ListLength(Expression *subject)
: subject_(subject)
{ subject_->set_parent(this); }

const Expression &ListLength::subject() const
{ return *subject_; }

string ListLength::to_string(const string &pfx) const
{ return "length(" + subject_->to_string(pfx) + ')'; }



string to_string(ListOpEnd which_end)
{
	switch (which_end) {
	case FRONT:
		return "front";
	case BACK:
		return "back";
	}
	throw Bug(string("Unhandled ") + typeid(which_end).name());
}



ListPop::ListPop(Expression *list, ListOpEnd which_end)
: list_(list)
, which_end_(which_end)
{ list_->set_parent(this); }

const Expression &ListPop::list() const
{ return *list_; }

ListOpEnd ListPop::which_end() const
{ return which_end_; }

string ListPop::to_string(const string &pfx) const
{ return pfx + "pop_" + gologpp::to_string(which_end_) + '(' + list_->str() + ')'; }



ListPush::ListPush(Expression *list, ListOpEnd which_end, Expression *what)
: list_(list)
, which_end_(which_end)
{
	list_->set_parent(this);

	what->ensure_type(
		dynamic_cast<const ListType &>(
			list_->type()
		).element_type()
	);
	what_.reset(what);
	what_->set_parent(this);
}

const Expression &ListPush::list() const
{ return *list_; }

ListOpEnd ListPush::which_end() const
{ return which_end_; }

const Expression &ListPush::what() const
{ return *what_; }

string ListPush::to_string(const string &pfx) const
{ return pfx + "push_" + gologpp::to_string(which_end_) + '(' + list_->str() + ')'; }



During::During(
	Reference<Action> *action_call,
	Instruction *parallel_block,
	boost::optional<Instruction *> on_fail,
	boost::optional<Instruction *> on_cancel
)
: action_call_(action_call)
, parallel_block_(parallel_block)
, on_fail_(on_fail.get_value_or(
	new Block(
		new Scope(parallel_block->scope().parent_scope()),
		{}
	)
  ))
, on_cancel_(on_cancel.get_value_or(
	new Block(
		new Scope(parallel_block->scope().parent_scope()),
		{}
	)
  ))
{
	action_call_->set_parent(this);
	parallel_block_->set_parent(this);
	on_fail_->set_parent(this);
	on_cancel_->set_parent(this);
}

const Reference<Action> &During::action_call() const
{ return *action_call_; }

const Instruction &During::parallel_block() const
{ return *parallel_block_; }

const Instruction &During::on_fail() const
{ return *on_fail_; }

const Instruction &During::on_cancel() const
{ return *on_cancel_; }

string During::to_string(const string &pfx) const
{
	return pfx + "during (" + action_call().str() + ")\n"
		+ parallel_block().to_string(pfx)
		+ "on_fail " + on_fail().to_string(pfx)
		+ "on_cancel " + on_cancel().to_string(pfx)
	;
}






} // namespace gologpp




