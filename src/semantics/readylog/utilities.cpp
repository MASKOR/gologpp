/*************************************************************************
 * This file is part of golog++.
 *
 * golog++ is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * golog++ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with golog++.  If not, see <https://www.gnu.org/licenses/>.
**************************************************************************/

#include "utilities.h"
#include "error.h"

#include <cstring>

namespace gologpp {


EC_word operator && (const EC_word &lhs, const EC_word &rhs)
{ return ::term(EC_functor(",", 2), lhs, rhs); }



template<>
EC_word to_ec_term(const string &functor, const vector<EC_word> &vec, vector<EC_word>::const_iterator begin)
{
	if (begin + 1 == vec.cend())
		return *begin;
	else
		return ::term(EC_functor(functor.c_str(), 2),
			*begin,
			to_ec_term(functor, vec, begin + 1)
		);
}



template<>
EC_word to_ec_list(const vector<EC_word> &vec, typename vector<EC_word>::const_iterator begin)
{
	if (begin == vec.cend())
		return ::nil();
	else
		return ::list(*begin, to_ec_list(vec, begin + 1));
}



EC_word copy_term(EC_word t)
{
	EC_functor fn;
	EC_atom atom;
	char *strval;
	long str_sz;
	long lval;
	long long llval;
	double dval;
	EC_word head, tail;

	if (t.is_nil() == EC_succeed)
		return ::nil();
	else if (t.is_list(head, tail) == EC_succeed) {
		EC_word hc = copy_term(head);
		EC_word tc = copy_term(tail);
		return ::list(hc, tc);
	}
	else if (t.is_atom(&atom) == EC_succeed)
		return atom;
	else if (t.functor(&fn) == EC_succeed) {
		vector<EC_word> args(size_t(fn.arity()));
		for (size_t i = 0; i < size_t(fn.arity()); ++i) {
			EC_word t_arg;
			if (t.arg(int(i+1), t_arg) != EC_succeed)
				throw std::runtime_error("Unable to retrieve term argument");

			args[i] = copy_term(t_arg);
		}

		return ::term(fn, args.data());
	} else if (t.is_string(&strval, &str_sz) == EC_succeed) {
		char *str_cp = new char[size_t(str_sz)+1];
		return EC_word(std::strncpy(str_cp, strval, size_t(str_sz)+1));
	} else if (t.is_long(&lval) == EC_succeed)
		return EC_word(lval);
	else if (t.is_long_long(&llval) == EC_succeed)
		return EC_word(llval);
	else if (t.is_double(&dval) == EC_succeed)
		return EC_word(dval);
	else if (t.is_var() == EC_succeed) {
		EC_word var = ::newvar();
		var.unify(t);
		return var;
	}
	else
		throw std::runtime_error("Unrecognized term type");
}


int ManagedTerm::count_ = 0;

ManagedTerm::ManagedTerm()
: index_(-1)
{}

ManagedTerm::ManagedTerm(EC_word w)
: index_(count_++)
{ save(w); }

ManagedTerm::ManagedTerm(const ManagedTerm &other)
: index_(count_++)
{ save(other); }


ManagedTerm::~ManagedTerm()
{
	if (index_ >= 0)
		clear();
}


void ManagedTerm::clear()
{
	post_goal(::term(EC_functor("retractall", 1),
		::term(EC_functor("managed_term", 2),
			EC_word(index_),
			::newvar()
		)
	));

	if (EC_resume() != EC_succeed)
		throw EngineError("failed to retract term");
}


void ManagedTerm::save(EC_word w)
{
	if (index_ < 0)
		throw EngineError("Uninitialized ManagedTerm");

	clear();

	post_goal(::term(EC_functor("asserta", 1),
		::term(EC_functor("managed_term", 2),
			EC_word(index_),
			w
		)
	));

	if (EC_resume() != EC_succeed)
		throw EngineError("failed to assert term");
}


ManagedTerm::operator EC_word () const
{
	if (index_ < 0)
		throw EngineError("Uninitialized ManagedTerm");

	EC_word t = ::newvar();
	EC_word q = ::term(EC_functor("managed_term", 2),
		EC_word(index_),
		t
	);
	post_goal(::term(EC_functor("once", 1), q));
	if (EC_resume() != EC_succeed)
		throw EngineError("failed to query ManagedTerm");

	return t;
}


ManagedTerm &ManagedTerm::operator= (const EC_word &other)
{
	if (index_ < 0)
		index_ = count_++;
	else
		clear();

	save(other);
	return *this;
}


ManagedTerm &ManagedTerm::operator= (const ManagedTerm &other)
{ return operator= (EC_word(other)); }




}
