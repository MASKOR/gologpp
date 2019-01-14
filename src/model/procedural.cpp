#include <functional>
#include <cstdlib>

#include "procedural.h"
#include "formula.h"
#include "execution.h"
#include "reference.h"

#include "user_error.h"

namespace gologpp {


Block::Block(Scope *own_scope, const vector<VoidExpression *> &elements)
: ScopeOwner(own_scope)
{
	for (VoidExpression *stmt : elements) {
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



Choose::Choose(Scope *own_scope, const vector<VoidExpression *> &alternatives)
: ScopeOwner(own_scope)
{
	for (VoidExpression *stmt : alternatives) {
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
	BooleanExpression *condition,
	VoidExpression *block_true,
	const boost::optional<VoidExpression *> &block_false
)
: condition_(condition)
, block_true_(block_true)
, block_false_(block_false.value_or(nullptr))
{
	condition_->set_parent(this);
	block_true_->set_parent(this);
	block_false_->set_parent(this);
}

const BooleanExpression &Conditional::condition() const
{ return *condition_; }

const VoidExpression &Conditional::block_false() const
{ return *block_false_; }

const VoidExpression &Conditional::block_true() const
{ return *block_true_; }


string Conditional::to_string(const string &pfx) const
{
	return linesep + pfx + "if (" + condition().to_string("") + ") " + block_true().to_string(pfx)
		+ (block_false_ ? pfx + linesep + "else" + block_false().to_string(pfx) : "");
}



Concurrent::Concurrent(Scope *own_scope, const vector<VoidExpression *> &procs)
: ScopeOwner(own_scope)
{
	for (VoidExpression *p : procs) {
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



Search::Search(VoidExpression *statement)
: statement_(statement)
{
	statement_->set_parent(this);
}

const VoidExpression &Search::statement() const
{ return *statement_; }

string Search::to_string(const string &pfx) const
{ return linesep + pfx + "search " + statement().to_string(pfx); }



Solve::Solve(
	NumericExpression *horizon,
	Reference<NumericFunction> *reward,
	VoidExpression *statement
)
: statement_(statement)
, horizon_(horizon)
, reward_(reward)
{
	horizon_->set_parent(this);
	reward_->set_parent(this);
}

const Statement &Solve::statement() const
{ return *statement_; }

const NumericExpression &Solve::horizon() const
{ return *horizon_; }

const Reference<NumericFunction> &Solve::reward() const
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



Test::Test(BooleanExpression *expression)
: expression_(expression)
{
	expression_->set_parent(this);
}

const BooleanExpression &Test::expression() const
{ return *expression_; }

string Test::to_string(const string &pfx) const
{ return linesep + pfx + "test (" + expression().to_string("") + ')'; }



While::While(BooleanExpression *expression, VoidExpression *statement)
: expression_(expression)
, statement_(statement)
{
	expression_->set_parent(this);
	statement_->set_parent(this);
}

const BooleanExpression &While::expression() const
{ return *expression_; }

const VoidExpression &While::statement() const
{ return *statement_; }

string While::to_string(const string &pfx) const
{ return linesep + pfx + "while (" + expression().to_string("") + ") " + statement().to_string(pfx); }





AbstractFunction::AbstractFunction(Scope *own_scope, const string &name, const vector<shared_ptr<AbstractVariable>> &args)
: Global(name, args)
, ScopeOwner(own_scope)
{}

AbstractFunction::~AbstractFunction()
{}

const VoidExpression &AbstractFunction::definition() const
{ return *definition_; }

void AbstractFunction::define(VoidExpression *definition)
{
	definition_.reset(definition);
	definition_->set_parent(this);
}

void AbstractFunction::compile(AExecutionContext &ctx)
{ ctx.compile(*this); }



DurativeCall::DurativeCall(DurativeCall::Type type, Reference<Action> *action)
: type_(type)
, action_(action)
{}

DurativeCall::Type DurativeCall::type() const
{ return type_; }

const Reference<Action> &DurativeCall::action() const
{ return *action_; }

string DurativeCall::to_string(const string &pfx) const
{ return linesep + pfx + gologpp::to_string(type()) + "(" + action().str() + ");"; }



string to_string(DurativeCall::Type type)
{
	switch (type) {
	case DurativeCall::Type::START:
		return "start";
	case DurativeCall::Type::FINISH:
		return "finish";
	case DurativeCall::Type::FAIL:
		return "fail";
	case DurativeCall::Type::STOP:
		return "stop";
	}
	throw Bug(string("Unhandled ") + typeid(type).name());
}



} // namespace gologpp
