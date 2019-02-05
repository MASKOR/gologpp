#ifndef READYLOG_HISTORY_H_
#define READYLOG_HISTORY_H_

#include "semantics.h"
#include "utilities.h"
#include <model/history.h>


namespace gologpp {


template<>
class Semantics<History> : public HistorySemantics {
public:
	Semantics(History &);
	virtual ~Semantics() override = default;

	virtual shared_ptr<Transition> get_last_transition() override;
	virtual void append_exog(shared_ptr<Grounding<AbstractAction>> exog) override;
	virtual void append_sensing_result(shared_ptr<Activity>) override;
	EC_word current_history();
	void set_current_history(EC_word h);
	bool has_changed() const;

private:
	string get_head_name(EC_word head);
	EC_word get_history_head();

	ManagedTerm readylog_history_;
	bool has_changed_;
};


} // namespace gologpp

#endif // READYLOG_HISTORY_H_
