#ifndef READYLOG_ACTION_H_
#define READYLOG_ACTION_H_

#include <eclipseclass.h>
#include "Implementation.h"

namespace gologpp {
/*namespace readylog {

class Action : public Translatable<generic::Action, tuple<EC_word, EC_word, EC_word>> {
public:
	Action(generic::Action &&action);
    
private:
	void init(tuple<EC_word, EC_word, EC_word> &cache);
};


class ActionImplementation : public gologpp::Implementation<generic::Action> {
public:
	virtual tuple<EC_word, EC_word, EC_word> translate();
};


} // namespace readylog*/

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

	void init(readylog::ExecutionContext &ctx);

private:
	const generic::Action &action_;
};


} // namespace gologpp


#endif