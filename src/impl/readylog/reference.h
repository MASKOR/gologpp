#ifndef READYLOG_REFERENCE_H_
#define READYLOG_REFERENCE_H_

#include "implementation.h"
#include <model/reference.h>

// Include headers for everything we might have a reference to (see term() method)
#include <model/action.h>
#include <model/fluent.h>
#include <model/procedural.h>

#include <eclipseclass.h>


namespace gologpp {

template<class GologT> class Reference;


template<class ExpressionT>
class Implementation<Reference<Variable<ExpressionT>>> : public ReadylogImplementation {
public:
	Implementation(Reference<Variable<ExpressionT>> &ref)
	: reference_(ref)
	{}

	EC_word term() override
	{ return reference_.target()->implementation().term(); }

private:
	Reference<Variable<ExpressionT>> &reference_;
};


template<class GologT>
class Implementation<Reference<GologT>> : public ReadylogImplementation {
public:
	Implementation(Reference<GologT> &ref)
	: reference_(ref)
	{}

	EC_word term() override
	{
		if (!reference_.bound())
			reference_.bind();

		return ::term(EC_functor(reference_.name().c_str(), reference_.arity()),
			translate_args(reference_.args())
		);
	}

private:
	Reference<GologT> &reference_;
};


} // namespace gologpp



#endif // READYLOG_REFERENCE_H_
