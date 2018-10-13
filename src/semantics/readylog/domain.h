#ifndef READYLOG_DOMAIN_H_
#define READYLOG_DOMAIN_H_

#include "semantics.h"
#include "utilities.h"

#include <model/domain.h>
#include <model/error.h>

namespace gologpp {

template<class ExprT>
class Semantics<Domain<ExprT>> : public ReadylogSemantics {
public:
	Semantics(const Domain<ExprT> &domain)
	: domain_(domain)
	{}

	virtual EC_word plterm() override
	{ return to_ec_list(domain_.elements()); }

private:
	const Domain<ExprT> &domain_;
};



}

#endif // READYLOG_DOMAIN_H_
