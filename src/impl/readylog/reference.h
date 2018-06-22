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


template<class GologT>
class Implementation<Reference<GologT>> : public ReadylogImplementation {
public:
	Implementation(const Reference<GologT> &ref)
	: reference_(ref)
	{}


	EC_word term() override
	{
		EC_word *args = new EC_word[reference_.args().size()];
		arity_t i = 0;
		for (const unique_ptr<Expression> &expr : reference_.args())
			args[i++] = dynamic_cast<ReadylogImplementation &>(expr->implementation()).term();
		return ::term(EC_functor(reference_.name().c_str(), reference_.arity()), args);
	}


private:
	const Reference<GologT> &reference_;
};



} // namespace gologpp



#endif // READYLOG_REFERENCE_H_
