#ifndef READYLOG_CONSTANT_H_
#define READYLOG_CONSTANT_H_

#include "semantics.h"

#include <model/semantics.h>
#include <model/variable.h>
#include <model/error.h>

#include <eclipseclass.h>


namespace gologpp {


template<class ExpressionT>
class Semantics<Constant<ExpressionT>> : public ReadylogSemantics {
public:
	Semantics(const Constant<ExpressionT> &value)
	: value_(value)
	{}

	virtual ~Semantics() override = default;

	virtual EC_word plterm() override;

private:
	const Constant<ExpressionT> &value_;
};


} // namespace gologpp

#endif // READYLOG_CONSTANT_H_
