#ifndef READYLOG_IMPLEMENTATION_H_
#define READYLOG_IMPLEMENTATION_H_

#include <gologpp/Implementation.h>

#include <eclipseclass.h>

namespace gologpp {

namespace generic {
class ExecutionContext;
}

namespace readylog {
class ExecutionContext;
}

class ReadylogExpression : public AbstractImplementation {
public:
	virtual ~ReadylogExpression() override = default;
	virtual EC_word term() = 0;
	virtual void apply_to(generic::ExecutionContext &c) override;
	virtual void apply_to(readylog::ExecutionContext &) = 0;
};


}

#endif