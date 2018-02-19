#ifndef READYLOG_ATOMS_H_
#define READYLOG_ATOMS_H_

#include "Implementation.h"

#include <gologpp/Implementation.h>
#include <gologpp/atoms.h>

#include <eclipseclass.h>

namespace gologpp {


template<>
class Implementation<Variable> : public ReadylogExpression {
public:
	Implementation(const Variable &);
	virtual ~Implementation() override = default;

	void init();
	virtual EC_word term() override;

private:
	const Variable &variable_;
	EC_word ec_var_;
};


template<>
class Implementation<AnyValue> : public ReadylogExpression {
public:
	Implementation(const AnyValue &);
	virtual ~Implementation() override = default;

	virtual EC_word term() override;

private:
	const AnyValue &value_;
};


} // namespace gologpp



#endif
