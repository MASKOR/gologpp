#include "utilities.h"
#include "atoms.h"

namespace gologpp {
namespace generic {

using namespace std;

Name::Name(const string &name)
: name_(name)
{}

Name::Name(Name &&other)
: name_(std::move(other.name_))
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


Scope Scope::global_scope_;


Scope::Scope(const vector<shared_ptr<Variable>> &variables, Scope &parent_scope)
: parent_scope_(parent_scope)
{
	for (const shared_ptr<Variable> &v : variables)
		variables_.emplace(v->name(), v);
}


Scope::Scope(const vector<string> &variables, Scope &parent_scope)
: parent_scope_(parent_scope)
{
	for (const string &name : variables)
		variables_.emplace(name, shared_ptr<Variable>(new Variable(name, *this)));
}


Scope::Scope(Scope &&other)
: parent_scope_(other.parent_scope_)
, variables_(std::move(other.variables_))
{}


shared_ptr<Variable> Scope::variable(const string &name)
{
	auto it = variables_.find(name);
	shared_ptr<Variable> rv;
	if (it != variables_.end())
		rv = it->second;
	else {
		rv.reset(new Variable(name, *this));
		variables_.emplace(name, rv);
	}
	return rv;
}


vector<shared_ptr<Expression>> Scope::variables(const vector<string> &names)
{
	vector<shared_ptr<Expression>> rv;
	for (const string &name : names)
		rv.push_back(variable(name));
	return rv;
}


} // namespace generic
} // namespace gologpp