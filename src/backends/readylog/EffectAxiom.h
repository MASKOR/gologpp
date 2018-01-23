#ifndef READYLOG_EFFECTAXIOM_H_
#define READYLOG_EFFECTAXIOM_H_

#include <gologpp/EffectAxiom.h>
#include <eclipseclass.h>

namespace gologpp {
namespace readylog {

class EffectAxiom : public Translatable<generic::EffectAxiom, EC_word> {
public:
	EffectAxiom(generic::EffectAxiom &&other);

	void init(EC_word &cache);
};

} /* namespace readylog */
} /* namespace gologpp */

#endif /* READYLOG_EFFECTAXIOM_H_ */
