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

using namespace std;



class Name {
public:
    Name(const string &name);
    Name(Name &&other);
    Name(const Name &other) = default;
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
    NameWithArity(const NameWithArity &other) = default;
    
    virtual ~NameWithArity() override;
    
    arity_t arity() const;
    virtual bool operator == (const NameWithArity &other) const;
    virtual size_t hash() const override;
    
private:
    const arity_t arity_;
};


class Variable;
class Expression;


class Scope : public virtual enable_shared_from_this<Scope> {
public:
	Scope(const vector<shared_ptr<Variable>> &variables, const shared_ptr<Scope> &parent_scope);
	Scope(const vector<string> &variables, const shared_ptr<Scope> &parent_scope);
	Scope(Scope &&);

	virtual ~Scope();

	shared_ptr<Variable> variable(const string &name);
	vector<shared_ptr<Variable>> variables(const vector<string> &names);
	shared_ptr<Scope> parent_scope();

	static shared_ptr<Scope> global_scope()
	{ return shared_ptr<Scope>(nullptr); }

private:
	shared_ptr<Scope> parent_scope_;
	unordered_map<string, shared_ptr<Variable>> variables_;
};


} // namespace generic
} // namespace gologpp



namespace std {

template<>
struct hash<gologpp::generic::NameWithArity> {
    size_t operator () (const gologpp::generic::NameWithArity &o) const
    { return o.hash(); }
};


} // namespace std



#endif // GOLOGPP_UTILITIES_H_