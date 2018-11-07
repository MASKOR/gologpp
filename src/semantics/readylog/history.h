#ifndef READYLOG_HISTORY_H_
#define READYLOG_HISTORY_H_

#include "semantics.h"
#include <model/history.h>


namespace gologpp {


template<>
class Semantics<History> : public HistorySemantics {
public:
	Semantics(History &);
	virtual ~Semantics() override = default;

	virtual void append_exog(ExogTransition &&exog) override;
	EC_word current_history();
	void set_current_history(EC_word h);

private:
	EC_word readylog_history_;
};


} // namespace gologpp

#endif // READYLOG_HISTORY_H_
