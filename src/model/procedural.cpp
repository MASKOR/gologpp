#include <functional>
#include <cstdlib>

#include "procedural.h"
#include "formula.h"
#include "execution.h"
#include "reference.h"
#include "expressions.h"

#include "user_error.h"
#include "error.h"

namespace gologpp {


Block::Block(Scope *own_scope, const vector<Expression *> &elements)
: ScopeOwner(own_scope)
{
	for (Expression *stmt : elements) {
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

const vector<SafeExprOwner<VoidType>> &Block::elements() const
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



Choose::Choose(Scope *own_scope, const vector<Expression *> &alternatives)
: ScopeOwner(own_scope)
{
	for (Expression *stmt : alternatives) {
		stmt->set_parent(this);
		alternatives_.emplace_back(stmt);
	}
}

const vector<SafeExprOwner<VoidType>> &Choose::alternatives() const
{ return alternatives_; }


void Choose::attach_semantics(SemanticsFactory &f)
{
	if (!semantics_) {
		semantics_ = f.make_semantics(*this);
		scope().attach_semantics(f);
		for (SafeExprOwner<VoidType> &stmt : alternatives_)
			stmt->attach_semantics(f);
	}
}


string Choose::to_string(const string &pfx) const
{
	return linesep + pfx + "choose {" + linesep
		+ concat_list(alternatives(), linesep, pfx + indent) + linesep
		+ pfx + "}" linesep;
}



Conditional::Conditional(
	Expression *condition,
	Expression *block_true,
	Expression *block_false
)
: condition_(condition)
, block_true_(block_true)
, block_false_(block_false)
{
	condition_->set_parent(this);
	block_true_->set_parent(this);
	block_false_->set_parent(this);
}

const Expression &Conditional::condition() const
{ return *condition_; }

const Expression &Conditional::block_false() const
{ return *block_false_; }

const Expression &Conditional::block_true() const
{ return *block_true_; }


string Conditional::to_string(const string &pfx) const
{
	return linesep + pfx + "if (" + condition().to_string("") + ") " + block_true().to_string(pfx)
		+ (block_false_ ? pfx + linesep + "else" + block_false().to_string(pfx) : "");
}



Concurrent::Concurrent(Scope *own_scope, const vector<Expression *> &procs)
: ScopeOwner(own_scope)
{
	for (Expression *p : procs) {
		p->set_parent(this);
		procs_.emplace_back(p);
	}
}


void Concurrent::attach_semantics(SemanticsFactory &f)
{
	semantics_ = f.make_semantics(*this);
	scope().attach_semantics(f);
	for (SafeExprOwner<VoidType> &p : procs_)
		p->attach_semantics(f);
}

const vector<SafeExprOwner<VoidType>> &Concurrent::procs() const
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
	Expression *statement
)
: ScopeOwner(own_scope)
, variable_(variable)
, statement_(statement)
{
	if (domain)
		for (Value *c : *domain) {
			ensure_type_equality(*variable, *c);
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

const Expression &Pick::statement() const
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

	set_implementation(f.make_semantics(*this));
}





Search::Search(Expression *statement)
: statement_(statement)
{
	statement_->set_parent(this);
}

const Expression &Search::statement() const
{ return *statement_; }

string Search::to_string(const string &pfx) const
{ return linesep + pfx + "search " + statement().to_string(pfx); }



Solve::Solve(
	Expression *horizon,
	Reference<Function> *reward,
	Expression *statement
)
: statement_(statement)
, horizon_(horizon)
, reward_(reward)
{
	horizon_->set_parent(this);
	reward_->set_parent(this);
}

const Expression &Solve::statement() const
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



While::While(Expression *expression, Expression *statement)
: expression_(expression)
, statement_(statement)
{
	expression_->set_parent(this);
	statement_->set_parent(this);
}

const Expression &While::expression() const
{ return *expression_; }

const Expression &While::statement() const
{ return *statement_; }

string While::to_string(const string &pfx) const
{ return linesep + pfx + "while (" + expression().to_string("") + ") " + statement().to_string(pfx); }



Return::Return(Expression *expr)
: expr_(expr)
{ expr_->set_parent(this); }

const Expression &Return::expression() const
{ return *expr_; }

string Return::to_string(const string &pfx) const
{ return linesep + pfx + "return " + expression().to_string("") + ';'; }



Function::Function(
	Scope *own_scope,
	const string &type_name,
	const string &name,
	const vector<shared_ptr<Variable>> &args
)
: Global(name, args)
, ScopeOwner(own_scope)
{ set_type_by_name(type_name); }


Function::Function(
	Scope *own_scope,
	const string &type_name,
	const string &name,
	const boost::optional<vector<shared_ptr<Variable>>> &args
)
: Global(name, args.get_value_or({}))
, ScopeOwner(own_scope)
{ set_type_by_name(type_name); }


const Expression &Function::definition() const
{ return *definition_; }

void Function::compile(AExecutionContext &ctx)
{ ctx.compile(*this); }

Reference<Function> *Function::make_ref(const vector<Expression *> &args)
{ return make_ref_<Function>(args); }

Expression *Function::ref(const vector<Expression *> &args)
{ return make_ref(args); }


void Function::define(Expression *definition)
{
	definition_ = definition;
	definition_->set_parent(this);
}


string Function::to_string(const string &pfx) const
{
	string fn;
	if (type() == gologpp::type<VoidType>())
		fn = "procedure ";
	else
		fn = type().name() + " function ";

	return linesep + pfx + fn + name() + '('
	+ concat_list(params(), ", ")
	+ ") " + definition().to_string(pfx);
}



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



string to_string(DurativeCall::Hook hook)
{
	switch (hook) {
	case DurativeCall::Hook::START:
		return "start";
	case DurativeCall::Hook::FINISH:
		return "finish";
	case DurativeCall::Hook::FAIL:
		return "fail";
	case DurativeCall::Hook::STOP:
		return "stop";
	}
	throw Bug(string("Unhandled ") + typeid(hook).name());
}



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




} // namespace gologpp




