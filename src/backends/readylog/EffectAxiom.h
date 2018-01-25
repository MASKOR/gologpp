#ifndef READYLOG_EFFECTAXIOM_H_
#define READYLOG_EFFECTAXIOM_H_

#include <eclipseclass.h>
#include "Implementation.h"

namespace gologpp {
namespace readylog {

/*class EffectAxiom : public Translatable<generic::EffectAxiom, EC_word> {
public:
	EffectAxiom(generic::EffectAxiom &&other);

	void init(EC_word &cache);
}; //*/

} // namespace readylog


namespace generic {
class EffectAxiom;
}

template<>
class Implementation<generic::EffectAxiom> : public ReadylogExpression {
	Implementation(const generic::EffectAxiom &);
	virtual ~Implementation() override = default;

	virtual EC_word term() override;
};

} /* namespace gologpp */

#endif /* READYLOG_EFFECTAXIOM_H_ */
