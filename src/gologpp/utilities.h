#ifndef GOLOGPP_UTILITIES_H_
#define GOLOGPP_UTILITIES_H_

#include "gologpp.h"

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <type_traits>


namespace gologpp {
namespace generic {

template<class T>
using vector = std::vector<T>;

template<class K, class V>
using unordered_map = std::unordered_map<K, V>;


class Name {
public:
    Name(const string &name);
    Name(Name &&other);
    Name(const Name &other) = default;
    virtual ~Name() = default;
    
    const string &name() const;
    virtual bool operator == (const Name &other) const;
    virtual size_t hash() const;
    
private:
    const string name_;
};


class Identifier : public Name {
public:
    Identifier(const string &name, arity_t arity);
    Identifier(Identifier &&other);
    Identifier(const Identifier &other) = default;
    
    virtual ~Identifier() override = default;
    
    arity_t arity() const;
    virtual bool operator == (const Identifier &other) const;
    virtual size_t hash() const override;

private:
    const arity_t arity_;
};


class Variable;
class Expression;


class Scope {
public:
	Scope(const vector<shared_ptr<Variable>> &variables, Scope &parent_scope);
	Scope(const vector<string> &variables, Scope &parent_scope);
	Scope(Scope &&);

	shared_ptr<Variable> variable(const string &name);
	vector<shared_ptr<Expression>> variables(const vector<string> &names);
	shared_ptr<Scope> parent_scope();

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


} // namespace generic
} // namespace gologpp



namespace std {

template<>
struct hash<gologpp::generic::Identifier> {
    size_t operator () (const gologpp::generic::Identifier &o) const
    { return o.hash(); }
};


} // namespace std



#endif // GOLOGPP_UTILITIES_H_