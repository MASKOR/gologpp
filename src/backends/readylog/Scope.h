#ifndef BACKENDS_READYLOG_SCOPE_H_
#define BACKENDS_READYLOG_SCOPE_H_

#include <gologpp/Implementation.h>
#include <gologpp/Scope.h>

#include <eclipseclass.h>

namespace gologpp {

template<>
class Implementation<Scope> : public AbstractImplementation {
public:
	Implementation(const Scope &);

	EC_word *variables(const vector<string> &names);
	void init_vars();

private:
	const Scope &scope_;
};

} // namespace gologpp

#endif // BACKENDS_READYLOG_SCOPE_H_
