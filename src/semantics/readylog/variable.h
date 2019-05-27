#ifndef READYLOG_VARIABLE_H_
#define READYLOG_VARIABLE_H_

#include "semantics.h"

#include <model/semantics.h>
#include <model/variable.h>
#include <model/error.h>

#include <eclipseclass.h>


namespace gologpp {



template<>
class Semantics<Variable> : public ReadylogSemantics {
public:
	Semantics(const Variable &var);
	virtual ~Semantics() override;

	void init();
	virtual EC_word plterm() override;
	void translate_as_golog_var(bool as_gv);
	EC_word member_restriction();

private:
	const Variable &var_;
	EC_word ec_var_;
	EC_atom golog_var_;
	bool as_golog_var_;
};



/**
 * Scope guard that changes a variable implementation to render itself as a
 * Golog variable instead of a Prolog variable
 */
class GologVarMutator {
public:
	GologVarMutator(Semantics<Variable> &var_impl);
	GologVarMutator(const Reference<Variable> &var_ref);
	~GologVarMutator();

private:
	Semantics<Variable> &var_impl_;
};



} // namespace gologpp

#endif // READYLOG_VARIABLE_H_
