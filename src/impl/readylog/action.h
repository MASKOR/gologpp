#ifndef READYLOG_ACTION_H_
#define READYLOG_ACTION_H_

#include "implementation.h"

#include <model/action.h>

#include <eclipseclass.h>

namespace gologpp {

class Action;


template<>
class Implementation<Action> : public ReadylogImplementation {
public:
	Implementation(const Action &a);
	virtual ~Implementation() override = default;

	EC_word prim_action();

	vector<EC_word> SSAs();

	EC_word prolog_poss_decl();
	EC_word prolog_poss();

	virtual EC_word term() override;
private:
	const Action &action_;
};


template<>
class Implementation<ExogAction> : public Implementation<AbstractLanguageElement> {
public:
	Implementation(const ExogAction &a);
	virtual ~Implementation() override = default;

	EC_word exog_action();
	vector<EC_word> SSAs();

	virtual EC_word term() override;

private:
	const ExogAction &exog_;
};


template<>
class Implementation<Transition> : public Implementation<AbstractLanguageElement> {
public:
	Implementation(const Transition &trans);
	virtual ~Implementation() override = default;

	virtual EC_word term() override;
private:
	const Transition &trans_;
};


template<>
class Implementation<ExogTransition> : public Implementation<AbstractLanguageElement> {
public:
	Implementation(const ExogTransition &trans);
	virtual ~Implementation() override = default;

	virtual EC_word term() override;
private:
	const ExogTransition &trans_;
};


} // namespace gologpp


#endif
