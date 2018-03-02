#ifndef READYLOG_FLUENT_H_
#define READYLOG_FLUENT_H_

#include <eclipseclass.h>
#include "Implementation.h"


namespace gologpp {


template<>
class Implementation<Fluent> : public ReadylogImplementation {
public:
	Implementation(const Fluent &);
	virtual ~Implementation() override = default;

	virtual EC_word term() override;

private:
	const Fluent *fluent_;
};


} // namespace gologpp



#endif // READYLOG_FLUENT_H_
