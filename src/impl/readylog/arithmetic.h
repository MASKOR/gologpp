#ifndef READYLOG_ARITHMETIC_H_
#define READYLOG_ARITHMETIC_H_

#include "semantics.h"
#include <eclipseclass.h>

namespace gologpp {

class ArithmeticOperation;

template<>
class Semantics<ArithmeticOperation> : public ReadylogSemantics {
public:
	Semantics(const ArithmeticOperation &);

	EC_word plterm() override;

private:
	const ArithmeticOperation &operation_;
	const char *functor_;
};


}

#endif // READYLOG_ARITHMETIC_H_
