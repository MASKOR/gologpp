#ifndef READYLOG_ACTION_H_
#define READYLOG_ACTION_H_

#include <eclipseclass.h>
#include "Implementation.h"

namespace gologpp {

namespace generic {
class Action;
}


namespace readylog {
class ExecutionContext;
}


template<>
class Implementation<generic::Action> : public ReadylogExpression {
public:
	Implementation(const generic::Action &a);
	virtual ~Implementation() override = default;

	virtual EC_word prim_action();
	virtual EC_word poss();
	virtual EC_word causes_val();

	virtual EC_word term() override;
private:
	const generic::Action &action_;
};


} // namespace gologpp


#endif
