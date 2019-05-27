#ifndef READYLOG_DOMAIN_H_
#define READYLOG_DOMAIN_H_

#include "semantics.h"
#include "utilities.h"

#include <model/domain.h>
#include <model/error.h>

namespace gologpp {

template<>
class Semantics<Domain> : public ReadylogSemantics {
public:
	Semantics(const Domain &domain);

	virtual EC_word plterm() override;

private:
	const Domain &domain_;
};



}

#endif // READYLOG_DOMAIN_H_
