#ifndef READYLOG_IMPLEMENTATION_H_
#define READYLOG_IMPLEMENTATION_H_

#include <gologpp/Implementation.h>

#include <eclipseclass.h>

namespace gologpp {

class ReadylogExpression : public AbstractImplementation {
public:
	virtual ~ReadylogExpression() = default;
	virtual EC_word term() = 0;
};


}

#endif