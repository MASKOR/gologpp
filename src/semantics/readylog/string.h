#ifndef READYLOG_STRING_CONCATENATION_H_
#define READYLOG_STRING_CONCATENATION_H_

#include "semantics.h"

#include <eclipseclass.h>

#include <model/string.h>

namespace gologpp {


template<>
class Semantics<ToString> : public ReadylogSemantics {
public:
	Semantics(const ToString &subject);

	EC_word plterm() override;

private:
	const ToString &to_string_;
};


template<>
class Semantics<StringConcatenation> : public ReadylogSemantics {
public:
	Semantics(const StringConcatenation &subject);

	EC_word plterm() override;

private:
	const StringConcatenation &subject_;
};


}

#endif // READYLOG_STRING_CONCATENATION_H_
