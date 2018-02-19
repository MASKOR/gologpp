#ifndef GOLOGPP_SCOPE_H_
#define GOLOGPP_SCOPE_H_

#include <vector>
#include <unordered_map>

#include "utilities.h"
#include "Language.h"

namespace gologpp {

template<class T>
using vector = std::vector<T>;

template<class K, class V>
using unordered_map = std::unordered_map<K, V>;


class Variable;
class Expression;


class Scope : public LanguageElement<Scope> {
public:
	Scope(const vector<shared_ptr<Variable>> &variables, Scope &parent_scope);
	Scope(const vector<string> &variables, Scope &parent_scope);

	Scope(Scope &&);

	Scope(const Scope &) = delete;
	Scope &operator = (const Scope &) = delete;

	shared_ptr<Variable> variable(const string &name) const;
	shared_ptr<Variable> variable(const string &name);
	vector<shared_ptr<Expression>> variables(const vector<string> &names) const;
	shared_ptr<Scope> parent_scope();

	void implement(Implementor &implementor);
	const unordered_map<string, shared_ptr<Variable>> &map() const;

	static Scope &global_scope()
	{ return global_scope_; }

private:
	Scope()
	: parent_scope_(*this)
	{}

	static Scope global_scope_;
	Scope &parent_scope_;
	unordered_map<string, shared_ptr<Variable>> variables_;
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
