#ifndef READYLOG_CONSTANT_H_
#define READYLOG_CONSTANT_H_

#include "semantics.h"

#include <model/semantics.h>
#include <model/variable.h>
#include <model/error.h>

#include <eclipseclass.h>


namespace gologpp {


template<>
class Semantics<Value> : public ReadylogSemantics {
public:
	Semantics(const Value &value);

	virtual ~Semantics() override = default;
	virtual EC_word plterm() override;

private:
	const Value &value_;
};


} // namespace gologpp

#endif // READYLOG_CONSTANT_H_
