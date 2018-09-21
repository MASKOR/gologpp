#ifndef READYLOG_UTILITIES_H_
#define READYLOG_UTILITIES_H_

#include <eclipseclass.h>
#include <model/gologpp.h>

namespace gologpp {


EC_word operator && (const EC_word &lhs, const EC_word &rhs);


template<class ListT>
EC_word *to_ec_words(const ListT &args)
{
	EC_word *rv = new EC_word[args.size()];
	arity_t i = 0;
	for (const auto &arg : args)
		rv[i++] = arg->semantics().plterm();
	return rv;
}


template<class ListT>
EC_word to_ec_list(const ListT &vec, typename ListT::const_iterator begin)
{
	if (begin == vec.cend())
		return ::nil();
	else
		return ::list((*begin)->semantics().plterm(), to_ec_list(vec, begin + 1));
}


EC_word copy_term(EC_word t);





} // namespace gologpp

#endif // READYLOG_UTILITIES_H_