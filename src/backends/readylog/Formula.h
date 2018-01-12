#ifndef READYLOG_FORMULA_H_
#define READYLOG_FORMULA_H_

#include <gologpp/Formula.h>
#include <eclipseclass.h>

namespace gologpp {
namespace readylog {


class Negation : public gologpp::Negation, public LanguageElement<EC_word> {
public:
	using gologpp::Negation::Negation;

	virtual EC_word define() override;
};

} // namespace readylog
} // namespace gologpp


#endif