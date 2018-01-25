#ifndef READYLOG_ATOMS_H_
#define READYLOG_ATOMS_H_

#include <eclipseclass.h>
#include <gologpp/Implementation.h>
#include "Implementation.h"

namespace gologpp {

namespace generic {
class Variable;
class AnyValue;
} // namespace readylog


template<>
class Implementation<generic::Variable> : public ReadylogExpression {
public:
	Implementation(const generic::Variable &);
	virtual ~Implementation() override = default;

	virtual EC_word term() override;

private:

};


template<>
class Implementation<generic::AnyValue> : public ReadylogExpression {
	Implementation(const generic::AnyValue &);
	virtual ~Implementation() override = default;

	virtual EC_word term() override;
};


} // namespace gologpp



#endif