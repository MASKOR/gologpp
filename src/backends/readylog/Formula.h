#ifndef READYLOG_FORMULA_H_
#define READYLOG_FORMULA_H_

#include <gologpp/Formula.h>
#include <gologpp/Translation.h>
#include <eclipseclass.h>

namespace gologpp {
namespace readylog {


class Negation : public Translatable<generic::Negation, EC_word> {
public:
	using Translatable<generic::Negation, EC_word>::Translatable;

private:
	void init(EC_word &cache);
};

} // namespace readylog
} // namespace gologpp


#endif