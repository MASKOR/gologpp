#ifndef GOLOGPP_EFFECTAXIOM_H_
#define GOLOGPP_EFFECTAXIOM_H_

#include "Formula.h"
#include "Fluent.h"
#include "Execution.h"
#include "Translation.h"
#include "utilities.h"
#include "Reference.h"
#include "Language.h"

namespace gologpp {
namespace generic {

class EffectAxiom : public LanguageElement<EffectAxiom> {
public:
	EffectAxiom(Reference<Action> &&action, Reference<Fluent> &&fluent, const shared_ptr<Expression> &value);
	EffectAxiom(EffectAxiom &&);
	virtual ~EffectAxiom();

	const Reference<Action> &action();
	const Reference<Fluent> &fluent();
	const Expression &value();

protected:
	Reference<Action> action_;
	Reference<Fluent> fluent_;
	shared_ptr<Expression> value_;
};


} // namespace generic
} // namespace gologpp

#endif /* GOLOGPP_EFFECTAXIOM_H_ */
