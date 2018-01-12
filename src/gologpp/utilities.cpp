#include "utilities.h"
#include "atoms.h"

namespace gologpp {

using namespace std;

Name::Name(const string &name)
: name_(name)
{}

Name::Name(Name &&other)
: name_(std::move(other.name_))
{}

Name::~Name()
{}

const string &Name::name() const
{ return name_; }

bool Name::operator == (const Name &other) const
{ return name() == other.name(); }

size_t Name::hash() const
{ return std::hash<string>{}(name()); }


NameWithArity::NameWithArity(const string &name, arity_t arity)
: Name(name), arity_(arity)
{}

NameWithArity::NameWithArity(NameWithArity &&other)
: Name(std::move(other))
, arity_(std::move(other.arity()))
{}

arity_t NameWithArity::arity() const
{ return arity_; }

bool NameWithArity::operator == (const NameWithArity &other) const
{ return name() == other.name() && arity() == other.arity(); }

size_t NameWithArity::hash() const
{ return Name::hash() ^ (std::hash<gologpp::arity_t>{}(arity()) << 1); }


InScope::InScope(const shared_ptr<Scope> &parent_scope)
: parent_scope_(parent_scope)
{}


InScope::InScope(InScope &parent_expr)
{
	try {
		Scope &parent_scope = dynamic_cast<Scope &>(parent_expr);
		parent_scope_ = parent_scope.shared_from_this();
	} catch(bad_cast &) {
		parent_scope_ = parent_expr.parent_scope();
	}
}


InScope::InScope(InScope &&other)
: parent_scope_(std::forward<shared_ptr<Scope>>(other.parent_scope_))
{}


shared_ptr<Scope> InScope::parent_scope()
{ return parent_scope_; }


Scope::Scope(const vector<shared_ptr<Variable>> &variables, const shared_ptr<Scope> &parent_scope)
: InScope(parent_scope)
{
	for (const shared_ptr<Variable> &v : variables)
		variables_.emplace(v->name(), v);
}


Scope::Scope(const vector<string> &variables, const shared_ptr<Scope> &parent_scope)
: InScope(parent_scope)
{
	for (const string &name : variables)
		variables_.emplace(name, shared_ptr<Variable>(new Variable(name, shared_ptr<Scope>(this))));
}


Scope::Scope(Scope &&other)
: InScope(std::move(other))
, variables_(std::move(other.variables_))
{}


shared_ptr<Variable> Scope::variable(const string &name)
{
	auto it = variables_.find(name);
	shared_ptr<Variable> rv;
	if (it != variables_.end())
		rv = it->second;
	else {
		rv.reset(new Variable(name, shared_from_this()));
		variables_.emplace(name, rv);
	}
	return rv;
}


} // namespace gologpp