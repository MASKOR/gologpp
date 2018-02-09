#ifndef READYLOG_PROCEDURE_H_
#define READYLOG_PROCEDURE_H_

#include <gologpp/Implementation.h>
#include <eclipseclass.h>

#include "Implementation.h"

namespace gologpp {

namespace generic {
class Procedure;
}

template<>
class Implementation<generic::Procedure> : public ReadylogExpression {
public:
	Implementation(const generic::Procedure &proc);

	virtual EC_word term() override;

private:
	const generic::Procedure &procedure_;
};



} // namespace gologpp


#endif // READYLOG_PROCEDURE_H_
