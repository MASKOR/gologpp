#ifndef READYLOG_FLUENT_H_
#define READYLOG_FLUENT_H_

#include "semantics.h"
#include <model/fluent.h>

#include <eclipseclass.h>


namespace gologpp {


template<>
class Semantics<InitialValue> : public ReadylogSemantics {
public:
	Semantics(const InitialValue &v);

	virtual ~Semantics() override = default;
	virtual EC_word plterm() override;

protected:
	const InitialValue &ival_;
};



template<>
class Semantics<Fluent> : public ReadylogSemantics {
public:
	Semantics(const Fluent &f);
	virtual ~Semantics() override = default;

	virtual EC_word plterm() override;
	vector<EC_word> initially();
	EC_word prim_fluent();

private:
	const Fluent &fluent_;
};



} // namespace gologpp



#endif // READYLOG_FLUENT_H_
