#ifndef READYLOG_REFERENCE_H_
#define READYLOG_REFERENCE_H_

#include "semantics.h"
#include "utilities.h"
#include <model/reference.h>

// Include headers for everything we might have a reference to (see plterm() method)
#include <model/action.h>
#include <model/fluent.h>
#include <model/procedural.h>

#include <eclipseclass.h>


namespace gologpp {

template<class GologT> class Reference;



template<class ExpressionT>
class Semantics<Reference<Variable<ExpressionT>>> : public ReadylogSemantics {
public:
	Semantics(Reference<Variable<ExpressionT>> &ref)
	: reference_(ref)
	{}

	virtual EC_word plterm() override
	{ return reference_.target()->semantics().plterm(); }

private:
	Reference<Variable<ExpressionT>> &reference_;
};



template<class GologT>
EC_word reference_term(const Reference<GologT> &ref)
{
	if (ref.arity() > 0)
		return ::term(EC_functor(ref.name().c_str(), ref.arity()),
			to_ec_words(ref.args()).data()
		);
	else
		return EC_atom(ref.name().c_str());
}



template<class GologT>
class Semantics<Reference<GologT>> : public ReadylogSemantics {
public:
	Semantics(Reference<GologT> &ref)
	: reference_(ref)
	{}

	virtual EC_word plterm() override
	{ return reference_term(reference_); }

private:
	Reference<GologT> &reference_;
};



template<>
EC_word Semantics<Reference<Action>>::plterm();



} // namespace gologpp



#endif // READYLOG_REFERENCE_H_
