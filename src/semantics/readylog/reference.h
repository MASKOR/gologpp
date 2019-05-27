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



template<>
class Semantics<Reference<Variable>> : public ReadylogSemantics {
public:
	Semantics(const Reference<Variable> &ref);

	virtual EC_word plterm() override;

private:
	const Reference<Variable> &ref_;
};



template<class GologT, class ExprT>
EC_word reference_term(const ReferenceBase<GologT, ExprT> &ref)
{
	if (ref.arity() > 0)
		return ::term(EC_functor(ref.name().c_str(), ref.arity()),
			to_ec_words(ref.args()).data()
		);
	else
		return EC_atom(ref.name().c_str());
}



template<class GologT, class ExprT>
class Semantics<ReferenceBase<GologT, ExprT>> : public Semantics<AbstractLanguageElement> {
public:
	Semantics(const ReferenceBase<GologT, ExprT> &ref)
	: ref_(ref)
	{}

	virtual EC_word plterm() override
	{ return reference_term(ref_); }

protected:
	const ReferenceBase<GologT, ExprT> &ref_;
};


template<>
EC_word Semantics<ReferenceBase<Action, Expression>>::plterm();



template<class TargetT>
class Semantics<Grounding<TargetT>> : public Semantics<ReferenceBase<TargetT, Constant>> {
public:
	using Semantics<ReferenceBase<TargetT, Constant>>::Semantics;
};



template<class TargetT>
class Semantics<Reference<TargetT>> : public Semantics<ReferenceBase<TargetT, Expression>> {
public:
	using Semantics<ReferenceBase<TargetT, Expression>>::Semantics;
};




} // namespace gologpp



#endif // READYLOG_REFERENCE_H_
