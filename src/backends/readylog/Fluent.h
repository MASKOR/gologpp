#ifndef READYLOG_FLUENT_H_
#define READYLOG_FLUENT_H_

#include <gologpp/Fluent.h>
#include <gologpp/Translation.h>
#include <eclipseclass.h>

namespace gologpp {
namespace readylog {


class Fluent : public Translatable<gologpp::Fluent, EC_word> {
public:
	using Translatable<gologpp::Fluent, EC_word>::Translatable;

	virtual EC_word translate() override;
};


} // namespace readylog
} // namespace gologpp



#endif // READYLOG_FLUENT_H_
