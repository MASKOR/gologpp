#include "utilities.h"

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

	if (t.is_list(head, tail) == EC_succeed)
		return ::list(copy_term(head), copy_term(tail));
	else if (t.functor(&fn) == EC_succeed) {
		EC_word *args = new EC_word[size_t(fn.arity())];
		for (size_t i = 0; i < size_t(fn.arity()); ++i) {
			EC_word t_arg;
			if (t.arg(int(i+1), t_arg) != EC_succeed)
				throw std::runtime_error("Unable to retrieve term argument");

			args[i] = copy_term(t_arg);
		}

		return ::term(fn, args);
	}
	else if (t.is_atom(&atom) == EC_succeed) {
		return atom;
	} else if (t.is_string(&strval, &str_sz) == EC_succeed) {
		char *str_cp = new char[size_t(str_sz)+1];
		return EC_word(std::strncpy(str_cp, strval, size_t(str_sz)+1));
	} else if (t.is_long(&lval) == EC_succeed)
		return EC_word(lval);
	else if (t.is_long_long(&llval) == EC_succeed)
		return EC_word(llval);
	else if (t.is_double(&dval) == EC_succeed)
		return EC_word(dval);
	else if (t.is_nil() == EC_succeed)
		return ::nil();
	else if (t.is_var() == EC_succeed)
		return t;
	else
		throw std::runtime_error("Unrecognized term type");
}

}