#include "history.h"
#include "utilities.h"
#include "action.h"
#include "execution.h"

#include <model/transition.h>


#include <iostream>
#include <unordered_map>

namespace gologpp {


Semantics<History>::Semantics(History &history)
: HistorySemantics(history)
, has_changed_(false)
{ readylog_history_ = ::list(EC_atom("s0"), ::nil()); }


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
	EC_atom head_atom;
	if (head.functor(&headfunctor) == EC_succeed)
		return headfunctor.name();
	else if (head.is_atom(&head_atom) == EC_succeed)
		return head_atom.name();

	throw Bug("Unknown term in history");
}




vector<unique_ptr<AbstractConstant>> get_args(EC_word head) {
	EC_word term;
	EC_atom did;
	double d;
	long i;
	char *s;

	vector<unique_ptr<AbstractConstant>> args;

	for (int j = 1; j <= head.arity(); j++) {
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
			throw Bug("Argument #" + std::to_string(j) + " is not a simple type");
	}

	return args;
}


static const std::unordered_map<string, Transition::Hook> name2state {
	{ "start", Transition::Hook::START },
	{ "stop", Transition::Hook::STOP },
	{ "finish", Transition::Hook::FINISH },
	{ "fail", Transition::Hook::FAIL },
};


shared_ptr<Transition> Semantics<History>::get_last_transition()
{
	if (!has_changed())
		return nullptr;

	EC_word head = get_history_head();
	string headname = get_head_name(head);
	auto state_it = name2state.find(headname);

	if (state_it == name2state.end())
		throw EngineError("Unhandled transition: " + headname);

	if (head.arity() != 2)
		throw EngineError("Transition arity must be 2: " + headname);

	head.arg(1, head);
	headname = get_head_name(head);

	vector<unique_ptr<AbstractConstant>> args = get_args(head);
	shared_ptr<Action> action = global_scope().lookup_global<Action>(headname, arity_t(head.arity()));
	shared_ptr<Transition> rv;

	return std::make_shared<Transition>(action, std::move(args), state_it->second);
}


void Semantics<History>::append_exog(shared_ptr<Grounding<Action>> trans)
{ set_current_history(::list(trans->semantics().plterm(), current_history())); }

EC_word Semantics<History>::current_history()
{ return readylog_history_; }

bool Semantics<History>::has_changed() const
{ return has_changed_; }

void Semantics<History>::set_current_history(EC_word h)
{
	ManagedTerm new_history(h);
	has_changed_ = ReadylogContext::instance().ec_query(::term(EC_functor(">", 2),
		::term(EC_functor("length", 1), new_history),
		::term(EC_functor("length", 1), current_history())
	));
	if (has_changed_)
		readylog_history_ = h;
}



} // namespace gologpp

