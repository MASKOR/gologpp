#ifndef READYLOG_EFFECTAXIOM_H_
#define READYLOG_EFFECTAXIOM_H_

#include "Implementation.h"

#include <gologpp/EffectAxiom.h>

#include <eclipseclass.h>

namespace gologpp {


template<>
class Implementation<EffectAxiom> : public ReadylogImplementation {
public:
	Implementation(const EffectAxiom &);
	virtual ~Implementation() override = default;

	virtual EC_word term() override;

private:
	const EffectAxiom &effect_;
};

} /* namespace gologpp */

#endif /* READYLOG_EFFECTAXIOM_H_ */
