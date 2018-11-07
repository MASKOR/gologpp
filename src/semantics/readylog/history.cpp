#include "history.h"
#include "utilities.h"
#include "action.h"

namespace gologpp {


Semantics<History>::Semantics(History &history)
: HistorySemantics(history)
{
	readylog_history_ = ::list(EC_atom("s0"), ::nil());
}


void Semantics<History>::append_exog(ExogTransition &&trans)
{
	readylog_history_ = ::list(trans.semantics().plterm(), readylog_history_);
}


EC_word Semantics<History>::current_history()
{
	return copy_term(readylog_history_);
}


void Semantics<History>::set_current_history(EC_word h)
{
	readylog_history_ = copy_term(h);
}


} // namespace gologpp