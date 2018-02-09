#ifndef READYLOG_FLUENT_H_
#define READYLOG_FLUENT_H_

#include <eclipseclass.h>
#include "Implementation.h"


namespace gologpp {

namespace generic {
class Fluent;
} // namespace generic


template<>
class Implementation<generic::Fluent> : public ReadylogExpression {
public:
	Implementation(const generic::Fluent &);
	virtual ~Implementation() override = default;

	virtual EC_word term() override;

private:
	const generic::Fluent &fluent_;
};


} // namespace gologpp



#endif // READYLOG_FLUENT_H_
