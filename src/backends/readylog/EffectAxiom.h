#ifndef READYLOG_EFFECTAXIOM_H_
#define READYLOG_EFFECTAXIOM_H_

#include <gologpp/EffectAxiom.h>
#include <eclipseclass.h>

namespace gologpp {
namespace readylog {

class EffectAxiom : public gologpp::Translatable<gologpp::EffectAxiom, EC_word> {
public:
	using gologpp::Translatable<gologpp::EffectAxiom, EC_word>::Translatable;

	EC_word translate() override;
};

} /* namespace readylog */
} /* namespace gologpp */

#endif /* READYLOG_EFFECTAXIOM_H_ */
