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


Value *pl_term_to_value(EC_word term) {
	EC_word type, list, list_head, list_tail;
	EC_atom did;
	EC_functor ftor;
	double d;
	long i;
	char *s;

	if (EC_succeed == term.is_long(&i))
		return new Value(NumberType::name(), i);
	else if (EC_succeed == term.is_double(&d))
		return new Value(NumberType::name(), d);
	else if (EC_succeed == term.is_atom(&did)) {
		if (did == EC_atom("true"))
			return new Value(BoolType::name(), true);
		else if (did == EC_atom("fail"))
			return new Value(BoolType::name(), false);
		else
			return new Value(SymbolType::name(), string(did.name()));
	}
	else if (EC_succeed == term.is_string(&s))
		return new Value(StringType::name(), string(s));
	else if (
		term.functor(&ftor) == EC_succeed
		&& ftor.name() == string("gpp_list")
		&& ftor.arity() == 2
		&& term.arg(1, type) == EC_succeed
		&& type.is_atom(&did) == EC_succeed
		&& term.arg(2, list) == EC_succeed
		&& list.is_list(list_head, list_tail) == EC_succeed
	) {
		vector<Value *> list_repr;
		do {
			list_repr.push_back(pl_term_to_value(list_head));
		} while (EC_succeed == list_tail.is_list(list_head, list_tail));

		string elem_t(did.name());
		elem_t = elem_t.substr(1); // remove # prefix that is added to avoid name clashes

		return new Value {
			"list[" + elem_t + "]",
			boost::optional<vector<Value *>> { list_repr }
		};
	}
	else if (
		term.functor(&ftor) == EC_succeed
		&& ftor.name() == string("gpp_compound")
		&& ftor.arity() == 2
		&& term.arg(1, type) == EC_succeed
		&& type.is_atom(&did) == EC_succeed
		&& term.arg(2, list) == EC_succeed
		&& list.is_list(list_head, list_tail) == EC_succeed
	) {
		vector<fusion_wtf_vector<string, Value *>> compound_repr;
		do {
			EC_functor field_ftor;
			EC_word field_value;
			if (
				list_head.functor(&field_ftor) == EC_succeed
				&& field_ftor.arity() == 1
				&& list_head.arg(0, field_value)
			) {
				compound_repr.push_back(
					fusion_wtf_vector<string, Value *> {
						string(field_ftor.name()), pl_term_to_value(list_head)
					}
				);
			}
			else
				return nullptr;
		} while (EC_succeed == list_tail.is_list(list_head, list_tail));

		string type_name(did.name());
		type_name = type_name.substr(1); // remove # prefix that is added to avoid name clashes

		return new Value(type_name, compound_repr);
	}

	else
		return nullptr;
}


vector<unique_ptr<Value>> get_args(EC_word head) {
	EC_word term;
	vector<unique_ptr<Value>> rv;

	for (int j = 1; j <= head.arity(); j++) {
		head.arg(j,term);
		Value *v = pl_term_to_value(term);

		if (!v)
			throw Bug("Invalid argument #" + std::to_string(j) + " in expression " + ReadylogContext::instance().to_string(head));

		rv.emplace_back(v);
	}

	return rv;
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
		return nullptr;

	if (head.arity() != 2)
		throw EngineError("Transition arity must be 2: " + headname);

	head.arg(1, head);
	headname = get_head_name(head);

	vector<unique_ptr<Value>> args = get_args(head);
	shared_ptr<Action> action = global_scope().lookup_global<Action>(headname, arity_t(head.arity()));
	shared_ptr<Transition> rv;

	return std::make_shared<Transition>(action, std::move(args), state_it->second);
}


void Semantics<History>::append_exog(shared_ptr<Grounding<AbstractAction>> trans)
{ set_current_history(::list(trans->semantics().plterm(), current_history())); }

void Semantics<History>::append_sensing_result(shared_ptr<Activity> a)
{ set_current_history(::list(a->semantics().sensing_result(), current_history())); }

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

