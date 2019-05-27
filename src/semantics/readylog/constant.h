#ifndef READYLOG_CONSTANT_H_
#define READYLOG_CONSTANT_H_

#include "semantics.h"

#include <model/semantics.h>
#include <model/variable.h>
#include <model/error.h>

#include <eclipseclass.h>


namespace gologpp {


template<>
class Semantics<Constant> : public ReadylogSemantics {
public:
	Semantics(const Constant &value);

	virtual ~Semantics() override = default;
	virtual EC_word plterm() override;

private:
	const Constant &value_;
};


} // namespace gologpp

#endif // READYLOG_CONSTANT_H_
