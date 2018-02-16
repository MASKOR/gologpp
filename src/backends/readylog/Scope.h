#ifndef BACKENDS_READYLOG_SCOPE_H_
#define BACKENDS_READYLOG_SCOPE_H_

#include <gologpp/Implementation.h>
#include <eclipseclass.h>
#include <gologpp/gologpp.h>

namespace gologpp {

namespace generic {
class Scope;
}

template<>
class Implementation<generic::Scope> : public AbstractImplementation {
public:
	Implementation(const generic::Scope &);

	EC_word *variables(const vector<string> &names);
	void init_vars();

private:
	const generic::Scope &scope_;
};

} // namespace gologpp

#endif // BACKENDS_READYLOG_SCOPE_H_
