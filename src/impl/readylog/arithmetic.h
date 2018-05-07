#ifndef READYLOG_ARITHMETIC_H_
#define READYLOG_ARITHMETIC_H_

#include "implementation.h"
#include <eclipseclass.h>

namespace gologpp {

class ArithmeticOperation;

template<>
class Implementation<ArithmeticOperation> : public ReadylogImplementation {
public:
	Implementation(const ArithmeticOperation &);

	EC_word term() override;

private:
	const ArithmeticOperation &operation_;
	const char *functor_;
};


}

#endif // READYLOG_ARITHMETIC_H_