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
	Implementation(Reference<GologT> &ref)
	: reference_(ref)
	{}

	EC_word term() override
	{ return reference_.target().implementation().term(); }

private:
	Reference<GologT> &reference_;
};



} // namespace gologpp



#endif // READYLOG_REFERENCE_H_
