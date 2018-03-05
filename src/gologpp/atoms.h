#ifndef GOLOGPP_ATOMS_H_
#define GOLOGPP_ATOMS_H_

#include "gologpp.h"

#include "utilities.h"
#include "Language.h"
#include "expressions.h"

#include <memory>

namespace gologpp {


class AbstractVariable : public Name, public AbstractLanguageElement {
public:
	AbstractVariable(const string &name, Scope &parent_scope);

private:
	Scope &parent_scope_;
};


template<class ExpressionT>
class Variable
: public ExpressionT
, public Name
, public LanguageElement<Variable<ExpressionT>>
, public AbstractLanguageElement
{
protected:
	Variable(const string &name, Scope &parent_scope)
	: AbstractVariable(name, parent_scope)
	{}

public:
	Variable(Variable<ExpressionT> &&) = default;
	Variable(const Variable<ExpressionT> &) = delete;
	Variable<ExpressionT> &operator = (Variable<ExpressionT> &&) = default;
	Variable<ExpressionT> &operator = (const Variable<ExpressionT> &) = delete;

	friend Scope;

	virtual ~Variable() override = default;
	DEFINE_IMPLEMENT
};


} // namespace gologpp



namespace std {

template<>
struct hash<shared_ptr<gologpp::AbstractVariable>> {
    size_t operator () (const shared_ptr<gologpp::AbstractVariable> &o) const
    { return o->hash(); }
};

} // namespace std

#endif
