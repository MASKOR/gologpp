#ifndef GOLOGPP_VARIABLE_H_
#define GOLOGPP_VARIABLE_H_

#include "gologpp.h"
#include "language.h"
#include "utilities.h"
#include "expressions.h"
#include "scope.h"
#include "domain.h"

namespace gologpp {


class Variable
: public LanguageElement<Variable>
, public Identifier
, public Expression
, public NoScopeOwner
, public std::enable_shared_from_this<Variable>
{
protected:
	Variable(const string &type_name, const string &name);

public:
	virtual ~Variable() override = default;

	friend Scope;

	Reference<Variable> *ref();

	Domain &domain();
	const Domain &domain() const;

	/**
	 * Add @param c to this Variable's @class Domain if it is an implicit domain.
	 * If it's an explicit domain, this method does nothing.
	 */
	virtual void add_implicit_domain_element(const Value &c);
	void set_domain(const string &domain_name);
	void set_domain_copy(const Domain &domain);
	virtual void define_implicit_domain(const string &domain_name);

	virtual string to_string(const string &) const override;

	DEFINE_ATTACH_SEMANTICS_WITH_MEMBERS(*domain_)

private:
	shared_ptr<Domain> domain_;
};


} // namespace gologpp



namespace std {

template<>
struct hash<shared_ptr<gologpp::Variable>> {
	size_t operator () (const shared_ptr<const gologpp::Variable> &o) const
	{ return o->hash(); }
};

template<>
struct equal_to<shared_ptr<gologpp::Variable>> {
	bool operator () (const shared_ptr<const gologpp::Variable> &lhs, const shared_ptr<const gologpp::Variable> &rhs) const
	{ return *lhs == *rhs; }
};

} // namespace std



#endif // GOLOGPP_VARIABLE_H_
