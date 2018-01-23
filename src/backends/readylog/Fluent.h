#ifndef READYLOG_FLUENT_H_
#define READYLOG_FLUENT_H_

#include <gologpp/Fluent.h>
#include <gologpp/Translation.h>
#include <eclipseclass.h>

#include <tuple>

namespace gologpp {
namespace readylog {

using namespace std;

class Fluent : public Translatable<generic::Fluent, EC_word> {
public:
	using Translatable<generic::Fluent, EC_word>::Translatable;

private:
	void init(EC_word &cache);
};


class Initially : public Translatable<generic::Initially, EC_word> {
public:
	using Translatable<generic::Initially, EC_word>::Translatable;

private:
	void init(EC_word &cache);
};


} // namespace readylog
} // namespace gologpp



#endif // READYLOG_FLUENT_H_
