#ifndef GOLOGPP_SCOPE_H_
#define GOLOGPP_SCOPE_H_

#include <vector>
#include <unordered_map>

#include "Language.h"
#include "utilities.h"
#include "atoms.h"

namespace gologpp {

template<class T>
using vector = std::vector<T>;

template<class K, class V>
using unordered_map = std::unordered_map<K, V>;


Scope &global_scope();


class Scope : public LanguageElement<Scope> {
public:
	Scope(const vector<shared_ptr<AbstractVariable>> &variables, Scope &parent_scope);

	Scope(Scope &&);

	Scope(const Scope &) = delete;
	Scope &operator = (const Scope &) = delete;

	template<class ExpressionT>
	shared_ptr<AbstractVariable> variable(const string &name)
	{
		auto it = variables_.find(name);
		shared_ptr<AbstractVariable> rv;
		if (it != variables_.end())
			rv = it->second;
		else {
			rv.reset(new Variable<ExpressionT>(name, *this));
			variables_.emplace(name, rv);
		}
		return rv;
	}

	shared_ptr<AbstractVariable> variable(const string &name) const;

	vector<shared_ptr<Expression>> variables(const vector<string> &names) const;
	shared_ptr<Scope> parent_scope();

	void implement(Implementor &implementor);
	const unordered_map<string, shared_ptr<AbstractVariable>> &map() const;

	static Scope &global_scope()
	{ return global_scope_; }

private:
	Scope()
	: parent_scope_(*this)
	{}

	static Scope global_scope_;
	Scope &parent_scope_;
	unordered_map<string, shared_ptr<AbstractVariable>> variables_;
};


} // namespace gologpp



namespace std {

template<>
struct hash<gologpp::Identifier> {
    size_t operator () (const gologpp::Identifier &o) const
    { return o.hash(); }
};


} // namespace std




#endif
