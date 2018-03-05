#include "Scope.h"
#include "atoms.h"

namespace gologpp {

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


shared_ptr<Variable> Scope::variable(const string &name) const
{
	auto it = variables_.find(name);
	shared_ptr<Variable> rv;
	if (it != variables_.end())
		rv = it->second;
	return rv;
}


vector<shared_ptr<Expression>> Scope::variables(const vector<string> &names) const
{
	vector<shared_ptr<Expression>> rv;
	for (const string &name : names)
		rv.push_back(variable(name));
	return rv;
}


void Scope::implement(Implementor &implementor)
{
	impl_ = implementor.make_impl(*this);
	for (auto &entry : variables_)
		entry.second->implement(implementor);
}


const unordered_map<string, shared_ptr<Variable>> &Scope::map() const
{ return variables_; }


} // namespace gologpp
