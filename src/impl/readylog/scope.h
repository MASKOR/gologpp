#ifndef BACKENDS_READYLOG_SCOPE_H_
#define BACKENDS_READYLOG_SCOPE_H_

#include <model/semantics.h>
#include <model/scope.h>

#include <eclipseclass.h>

namespace gologpp {


template<>
class Semantics<Scope> : public AbstractSemantics {
public:
	Semantics(const Scope &);

	EC_word *variables(const vector<string> &names);
	void init_vars();

private:
	const Scope &scope_;
};


} // namespace gologpp

#endif // BACKENDS_READYLOG_SCOPE_H_
