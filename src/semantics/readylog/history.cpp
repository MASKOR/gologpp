#include "history.h"
#include "utilities.h"
#include "action.h"

namespace gologpp {


EC_word Semantics<History>::get_history_head()
{
	EC_word head, tail;
	if (current_history().is_list(head, tail) != EC_succeed)
		throw Bug("ReadyLog history is not a list");
	return head;
}


string Semantics<History>::get_head_name(EC_word head)
{
	EC_functor headfunctor;
	if (head.functor(&headfunctor) == EC_succeed) {
		return headfunctor.name();
	}
	return "";
}


shared_ptr<Transition> Semantics<History>::get_last_transition()
{
	EC_word head = get_history_head();
	string headname = get_head_name(head);

	double d;
	long i;
	EC_atom did;
	char *s;
	EC_word term;

	vector<unique_ptr<AbstractConstant>> args;
	for (int j = 1; j <= head.arity();j++) {
		head.arg(j,term);
		if (EC_succeed == term.is_double(&d))
			args.emplace_back(new NumericConstant(d));
		else if (EC_succeed == term.is_long(&i))
			args.emplace_back(new NumericConstant(i));
		else if (EC_succeed == term.is_atom(&did)) {
			if (did == EC_atom("true"))
				args.emplace_back(new BooleanConstant(true));
			else if (did == EC_atom("fail"))
				args.emplace_back(new BooleanConstant(false));
			else
				args.emplace_back(new SymbolicConstant(string(did.name())));
		}
		else if (EC_succeed == term.is_string(&s))
			args.emplace_back(new StringConstant(string(s)));
		else
			throw Bug("Transition argument #" + std::to_string(j) + " is not a simple type");
	}

	shared_ptr<Action> on_shared = global_scope().lookup_global<Action>(headname, (arity_t)head.arity());
	shared_ptr<Transition> trans = std::make_shared<Transition>(on_shared,std::move(args));
	return trans;
}


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
