#ifndef READYLOG_ACTION_H_
#define READYLOG_ACTION_H_

#include "Implementation.h"

#include <gologpp/Action.h>

#include <eclipseclass.h>

namespace gologpp {

class Action;


template<>
class Implementation<Action> : public ReadylogExpression {
public:
	Implementation(const Action &a);
	virtual ~Implementation() override = default;

	virtual EC_word prim_action();
	virtual EC_word poss();
	virtual EC_word causes_val();

	virtual EC_word term() override;
private:
	const Action &action_;
};


} // namespace gologpp


#endif
