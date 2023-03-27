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
#include "reference.h"
#include "expressions.h"

#include <execution/controller.h>

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

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

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

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

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

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

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

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

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
	variable_->set_parent(this);
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

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

Search::Search(Instruction *statement)
: statement_(statement)
{
	statement_->set_parent(this);
}

const Instruction &Search::statement() const
{ return *statement_; }

string Search::to_string(const string &pfx) const
{ return linesep + pfx + "search " + statement().to_string(pfx); }

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

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

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

Test::Test(Expression *expression)
: expression_(expression)
{
	expression_->set_parent(this);
}

const Expression &Test::expression() const
{ return *expression_; }

string Test::to_string(const string &pfx) const
{ return pfx + "test (" + expression().to_string("") + ");"; }

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

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

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

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


void Function::compile(AExecutionController &ctx)
{ ctx.compile(*this); }

Expression *Function::ref(const vector<Expression *> &args)
{ return make_ref(args); }

Reference<Function> *Function::make_ref(const vector<Expression *> &args)
{ return make_ref_<Function>(args); }

const Expression &Function::definition() const
{ return *definition_; }

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

ExogFunction::ExogFunction(
	Scope *own_scope,
	const Type &t,
	const string &name,
	const vector<shared_ptr<Variable>> &args
)
: ScopeOwner(own_scope)
, Signified<Expression>(name, args)
{ set_type(t); }


ExogFunction::ExogFunction(
	Scope *own_scope,
	const Type &t,
	const string &name,
	const boost::optional<vector<shared_ptr<Variable>>> &args
)
: ScopeOwner(own_scope)
, Signified<Expression>(name, args.get_value_or({}))
{ set_type(t); }


string ExogFunction::to_string(const string &pfx) const
{
	return linesep + pfx + type().name() + " function " + name() + '('
		+ concat_list(params(), ", ")
		+ ");";
}


void ExogFunction::define()
{}

void ExogFunction::compile(AExecutionController &ctx)
{ ctx.compile(*this); }

Expression *ExogFunction::ref(const vector<Expression *> &args)
{ return make_ref(args); }

Reference<ExogFunction> *ExogFunction::make_ref(const vector<Expression *> &args)
{ return make_ref_<ExogFunction>(args); }



GeneralSemantics<Reference<ExogFunction>>::GeneralSemantics(const Reference<ExogFunction> &elem, AExecutionController &context)
: element_(&elem)
, context_(context)
{}

const Reference<ExogFunction> &GeneralSemantics<Reference<ExogFunction>>::element() const
{ return *element_; }

AExecutionController &GeneralSemantics<Reference<ExogFunction>>::context() const
{ return context_; }

void GeneralSemantics<Reference<ExogFunction>>::update_element(const Reference<ExogFunction> *new_element)
{ element_ = new_element; }

const ModelElement &GeneralSemantics<Reference<ExogFunction>>::model_element() const
{ return *element_; }

const Expression &GeneralSemantics<Reference<ExogFunction>>::expression() const
{ return *element_; }


Value GeneralSemantics<Reference<ExogFunction> >::evaluate(const BindingChain &b, const History &h)
{
	std::unordered_map<string, Value> backend_args;

	BindingChain merged(b);
	merged.push_back(&element().binding());

	for (auto &param : element()->params()) {
		unique_ptr<Reference<Variable>> var_ref(param->ref());
		var_ref->attach_semantics(context().semantics_factory());
		backend_args.emplace(
			param->name(),
			var_ref->general_semantics().evaluate(merged, h)
		);
	}

	Value rv = context().backend().eval_exog_function(
		element().type(),
		element().name(),
		backend_args
	);
	rv.attach_semantics(this->context().semantics_factory());
	return rv;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

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


void Procedure::compile(AExecutionController &ctx)
{ ctx.compile(*this); }

Reference<Procedure> *Procedure::make_ref(const vector<Expression *> &args)
{ return make_ref_<Procedure>(args); }

Instruction *Procedure::ref(const vector<Expression *> &args)
{ return make_ref(args); }

const Instruction &Procedure::definition() const
{ return *definition_; }

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

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


string to_string(DurativeCall::Hook h)
{
	switch (h) {
	case Transition::Hook::START:
		return "start";
	case Transition::Hook::CANCEL:
		return "cancel";
	case Transition::Hook::FINISH:
		return "finish";
	case Transition::Hook::FAIL:
		return "fail";
	case Transition::Hook::END:
		return "end";
	}
	throw Bug(string("Unhandled ") + typeid(h).name());
}

/////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////

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




