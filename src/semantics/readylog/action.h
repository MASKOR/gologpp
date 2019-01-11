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

	EC_word durative_action();
	vector<EC_word> durative_causes_vals();
	//vector<EC_word> SSAs();
	//vector<EC_word> prolog_poss_decls();
	//vector<EC_word> prolog_poss();
	EC_word durative_poss();
	//EC_word get_durative_poss(const string &which);

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
class Semantics<Activity> : public Semantics<AbstractLanguageElement> {
public:
	Semantics(const Activity &trans);
	virtual ~Semantics() override = default;

	virtual EC_word plterm() override;
private:
	const Activity &trans_;
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


} // namespace gologpp


#endif
