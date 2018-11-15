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

	virtual shared_ptr <Transition> get_last_transition() override;
	virtual void append_exog(ExogTransition &&exog) override;
	EC_word current_history();
	void set_current_history(EC_word h);

private:
	string get_head_name(EC_word head);
	EC_word get_history_head();

	EC_word readylog_history_;
};


} // namespace gologpp

#endif // READYLOG_HISTORY_H_
