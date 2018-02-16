#ifndef READYLOG_REFERENCE_H_
#define READYLOG_REFERENCE_H_

#include "Implementation.h"
#include <gologpp/Reference.h>

// Include headers for everything we might have a reference to (see term() method)
#include <gologpp/Action.h>
#include <gologpp/Fluent.h>
#include <gologpp/Procedure.h>

#include <eclipseclass.h>


namespace gologpp {

namespace generic {
template<class GologT> class Reference;
} // namespace generic


template<class GologT>
class Implementation<generic::Reference<GologT>> : public ReadylogExpression {
public:
	Implementation(const generic::Reference<GologT> &ref)
	: reference_(ref)
	{}


	EC_word term() override
	{ return dynamic_cast<ReadylogExpression &>(reference_->implementation()).term(); }


private:
	const generic::Reference<GologT> &reference_;
};



} // namespace gologpp



#endif // READYLOG_REFERENCE_H_
