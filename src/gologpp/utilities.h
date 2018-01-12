#ifndef GOLOGPP_UTILITIES_H_
#define GOLOGPP_UTILITIES_H_

#include "gologpp.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <type_traits>


namespace gologpp {

using namespace std;


template<class ReprT>
class LanguageElement {
public:
    virtual ReprT define() = 0;
};


class Name {
public:
    Name(const string &name);
    Name(Name &&other);
    virtual ~Name();
    
    const string &name() const;
    virtual bool operator == (const Name &other) const;
    virtual size_t hash() const;
    
private:
    const string name_;
};


class NameWithArity : public Name {
public:
    NameWithArity(const string &name, arity_t arity);
    NameWithArity(NameWithArity &&other);
    
    virtual ~NameWithArity() override;
    
    arity_t arity() const;
    virtual bool operator == (const NameWithArity &other) const;
    virtual size_t hash() const override;
    
private:
    const arity_t arity_;
};


class Variable;
class Scope;


class InScope {
public:
	InScope(const shared_ptr<Scope> &parent_scope);
	InScope(InScope &parent_expr);
	InScope(InScope &&other);

	virtual ~InScope();
	shared_ptr<Scope> parent_scope();

private:
	shared_ptr<Scope> parent_scope_;
};


class Scope : public enable_shared_from_this<Scope>, public InScope {
public:
	Scope(const vector<shared_ptr<Variable>> &variables, const shared_ptr<Scope> &parent_scope);
	Scope(const vector<string> &variables, const shared_ptr<Scope> &parent_scope);
	Scope(Scope &&);

	virtual ~Scope();

	shared_ptr<Variable> variable(const string &name);

	static shared_ptr<Scope> global_scope()
	{ return shared_ptr<Scope>(nullptr); }

private:
	unordered_map<string, shared_ptr<Variable>> variables_;
};


} // namespace gologpp



namespace std {

template<>
struct hash<gologpp::NameWithArity> {
    size_t operator () (const gologpp::NameWithArity &o) const
    { return o.hash(); }
};


} // namespace std



#endif // GOLOGPP_UTILITIES_H_