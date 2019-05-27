#ifndef READYLOG_ACTION_H_
#define READYLOG_ACTION_H_

#include "semantics.h"
#include "reference.h"

#include <model/action.h>
#include <model/transition.h>

#include <eclipseclass.h>

namespace gologpp {

class Action;


template<>
class Semantics<Action> : public ReadylogSemantics {
public:
	Semantics(const Action &a);
	virtual ~Semantics() override = default;

	EC_word durative_action();
	vector<EC_word> durative_causes_vals();
	EC_word durative_poss();
	EC_word senses();

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
	vector<EC_word> causes_vals();
	EC_word poss();

	virtual EC_word plterm() override;

private:
	const ExogAction &exog_;
};


template<>
class Semantics<ExogEvent> : public Semantics<Grounding<ExogAction>> {
public:
	using Semantics<Grounding<ExogAction>>::Semantics;
	virtual ~Semantics() override = default;
};


template<>
class Semantics<Activity> : public Semantics<Grounding<Action>> {
public:
	using Semantics<Grounding<Action>>::Semantics;
	virtual ~Semantics() override = default;

	const Activity &activity();

	virtual EC_word plterm() override;
	EC_word sensing_result();
};


template<>
class Semantics<Transition> : public Semantics<Grounding<Action>> {
public:
	using Semantics<Grounding<Action>>::Semantics;
	virtual ~Semantics() override = default;

	const Transition &trans();

	virtual EC_word plterm() override;
};


} // namespace gologpp


#endif
