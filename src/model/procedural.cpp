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
		stmt->ensure_type<Void>();
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

const vector<unique_ptr<VoidExpression>> &Block::elements() const
{ return elements_; }

string Block::to_string(const string &pfx) const
{
	if (elements().size() == 0)
		return "{}";
	else if (elements().size() == 1)
		return elements()[0]->to_string(pfx);
	else {
		return pfx + "{" + linesep
			+ concat_list(elements(), ";" linesep + pfx + indent, pfx + indent) + ";" linesep
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

const vector<unique_ptr<VoidExpression>> &Choose::alternatives() const
{ return alternatives_; }


void Choose::attach_semantics(SemanticsFactory &f)
{
	if (!semantics_) {
		semantics_ = f.make_semantics(*this);
		scope().attach_semantics(f);
		for (unique_ptr<VoidExpression> &stmt : alternatives_)
			stmt->attach_semantics(f);
	}
}


string Choose::to_string(const string &pfx) const
{
	return linesep + pfx + "choose {" + linesep
		+ concat_list(alternatives(), ";" linesep + pfx + indent, pfx + indent) + ";" linesep
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
		p->ensure_type<Void>();
		p->set_parent(this);
		procs_.emplace_back(p);
	}
}


void Concurrent::attach_semantics(SemanticsFactory &f)
{
	semantics_ = f.make_semantics(*this);
	scope().attach_semantics(f);
	for (unique_ptr<VoidExpression> &p : procs_)
		p->attach_semantics(f);
}

const vector<unique_ptr<VoidExpression>> &Concurrent::procs() const
{ return procs_; }


string Concurrent::to_string(const string &pfx) const
{
	return linesep + pfx + "concurrent {" + linesep
		+ concat_list(procs(), ";" linesep + pfx + indent, pfx + indent) + ";" linesep
		+ pfx + "}";
}



Pick::Pick(
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


const vector<unique_ptr<Constant>> &Pick::domain() const
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

	for (unique_ptr<Constant> & c : domain_)
		c->attach_semantics(f);
	variable_->attach_semantics(f);
	statement_->attach_semantics(f);

	set_implementation(f.make_semantics(*this));
}





Search::Search(Expression *statement)
: statement_(statement)
{
	statement->ensure_type<Void>();
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
{ return linesep + pfx + "test (" + expression().to_string("") + ')'; }



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
	if (type().dynamic_tag() == ExpressionTypeTag::VOID)
		fn = "procedure ";
	else
		fn = gologpp::to_string(type().dynamic_tag()) + "function ";

	return linesep + pfx + fn + name() + '('
	+ concat_list(args(), ", ")
	+ ") " + definition().to_string(pfx);
}



DurativeCall::DurativeCall(DurativeCall::Hook type, Reference<Action> *action)
: hook_(type)
, action_(action)
{}

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
: field_name_(field_name)
{
	subject_ = subject;
	if (!set_type(subject_->type().field_type(field_name)))
		throw Bug("Failed to set type");
}

const Expression &FieldAccess::subject() const
{ return *subject_; }

const string &FieldAccess::field_name() const
{ return field_name_; }

string FieldAccess::to_string(const string &pfx) const
{ return pfx + subject().str() + "." + gologpp::to_string(type().dynamic_tag()) + field_name(); }



FieldAccess *nested_field_access_(
	Expression *subject,
	vector<string>::const_iterator it,
	vector<string>::const_iterator end)
{
	if (it + 1 < end)
		return new FieldAccess(
			nested_field_access_(subject, it + 1, end),
			*it
		);
	else
		return new FieldAccess(subject, *it);
}


FieldAccess *nested_field_access(Expression *subject, const vector<string> &fields)
{ return nested_field_access_(subject, fields.begin(), fields.end()); }



} // namespace gologpp

