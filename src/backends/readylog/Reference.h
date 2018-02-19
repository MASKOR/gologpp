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
	{
		EC_word *args = new EC_word[reference_.args().size()];
		arity_t i = 0;
		for (const shared_ptr<generic::Expression> &exp : reference_.args())
			args[i++] = dynamic_cast<ReadylogExpression &>(exp->implementation()).term();
		return ::term(EC_functor(reference_.target().name().c_str(), reference_.target().arity()), args);
	}


private:
	const generic::Reference<GologT> &reference_;
};



} // namespace gologpp



#endif // READYLOG_REFERENCE_H_
