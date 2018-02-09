#ifndef GOLOGPP_EFFECTAXIOM_H_
#define GOLOGPP_EFFECTAXIOM_H_

#include "utilities.h"
#include "Reference.h"
#include "Language.h"

namespace gologpp {
namespace generic {

class Action;
class Expression;
class Fluent;

class EffectAxiom : public LanguageElement<EffectAxiom> {
public:
	EffectAxiom(Reference<Action> &&action, Reference<Fluent> &&fluent, const shared_ptr<Expression> &value);
	EffectAxiom(EffectAxiom &&);
	virtual ~EffectAxiom() override;

	const Reference<Action> &action() const;
	const Reference<Fluent> &fluent() const;
	const Expression &value() const;

protected:
	Reference<Action> action_;
	Reference<Fluent> fluent_;
	shared_ptr<Expression> value_;
};


} // namespace generic
} // namespace gologpp

#endif /* GOLOGPP_EFFECTAXIOM_H_ */
