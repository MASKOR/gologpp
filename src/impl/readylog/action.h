#ifndef READYLOG_ACTION_H_
#define READYLOG_ACTION_H_

#include "semantics.h"

#include <model/action.h>

#include <eclipseclass.h>

namespace gologpp {

class Action;


template<>
class Semantics<Action> : public ReadylogSemantics {
public:
	Semantics(const Action &a);
	virtual ~Semantics() override = default;

	EC_word prim_action();

	vector<EC_word> SSAs();

	EC_word prolog_poss_decl();
	EC_word prolog_poss();

	virtual EC_word plterm() override;
private:
	const Action &action_;
};


template<>
class Semantics<ExogAction> : public Semantics<AbstractLanguageElement> {
public:
	Semantics(const ExogAction &a);
	virtual ~Semantics() override = default;

	EC_word exog_action();
	vector<EC_word> SSAs();

	virtual EC_word plterm() override;

private:
	const ExogAction &exog_;
};


template<>
class Semantics<Transition> : public Semantics<AbstractLanguageElement> {
public:
	Semantics(const Transition &trans);
	virtual ~Semantics() override = default;

	virtual EC_word plterm() override;
private:
	const Transition &trans_;
};


template<>
class Semantics<ExogTransition> : public Semantics<AbstractLanguageElement> {
public:
	Semantics(const ExogTransition &trans);
	virtual ~Semantics() override = default;

	virtual EC_word plterm() override;
private:
	const ExogTransition &trans_;
};


} // namespace gologpp


#endif
