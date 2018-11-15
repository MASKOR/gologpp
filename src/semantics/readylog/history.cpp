#include "history.h"
#include "utilities.h"
#include "action.h"

#include <iostream>

namespace gologpp {


EC_word Semantics<History>::get_history_head()
{
	EC_word head, tail;
	if (current_history().is_list(head, tail) != EC_succeed)
		std::cout << EC_fail << std::endl;
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
	EC_word head;
	string headname;

	head = get_history_head();
	headname = get_head_name(head);
	double d;
	long i;
	EC_atom did;
	char *s;
	EC_word term;
	vector<unique_ptr<AbstractConstant>> args;
	for(int j = 1; j <= head.arity();j++){
		head.arg(j,term);
		if (EC_succeed == term.is_double(&d)){
			args.emplace_back(new NumericConstant(d));
		}
		else if (EC_succeed == term.is_long(&i))
			std::cout << i << "\n";
		else if (EC_succeed == term.is_atom(&did))
			std::cout << did.Name() << "\n";
		else if (EC_succeed == term.is_string(&s))
			std::cout << s << "\n";
		else
			std::cout << "not a simple type\n";
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
