#ifndef READYLOG_FLUENT_H_
#define READYLOG_FLUENT_H_

#include <eclipseclass.h>
#include "Implementation.h"


namespace gologpp {


template<class ExpressionT>
class Implementation<Fluent<ExpressionT>> : public ReadylogImplementation {
public:
	Implementation(const Fluent<ExpressionT> &f)
	: fluent_(f)
	{}

	virtual ~Implementation() override = default;

	virtual EC_word term() override
	{
		return ::term(
			EC_functor(fluent_.name().c_str(), fluent_.arity()),
			fluent_.scope().implementation().variables(fluent_.args())
		);
	}

private:
	const Fluent<ExpressionT> &fluent_;
};


} // namespace gologpp



#endif // READYLOG_FLUENT_H_
