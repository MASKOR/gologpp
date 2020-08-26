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

#ifndef READYLOG_UTILITIES_H_
#define READYLOG_UTILITIES_H_

#include <eclipseclass.h>
#include <model/gologpp.h>
#include <model/error.h>
#include <model/expressions.h>
#include <type_traits>


namespace gologpp {


EC_word operator && (const EC_word &lhs, const EC_word &rhs);


template<class T> inline
T &access(T *ptr)
{ return *ptr; }

template<class T> inline
T &access(const unique_ptr<T> &ptr)
{ return *ptr; }

template<class T> inline
const Expression &access(const SafeExprOwner<T> &ptr)
{ return *ptr; }

template<class T> inline
typename std::enable_if<!std::is_pointer<T>::value, T>::type
&access(T &&ptr)
{ return std::forward<T>(ptr); }


template<class ListT>
vector<EC_word> to_ec_words(const ListT &args)
{
	vector<EC_word> rv;
	for (const auto &arg : args)
		rv.push_back(arg->semantics().plterm());
	return rv;
}

template<>
vector<EC_word> to_ec_words(const vector<shared_ptr<Variable>> &args);



template<class ListT>
EC_word to_ec_term(const string &functor, const ListT &vec, typename ListT::const_iterator begin)
{
	auto it_cur = begin;
	auto it_next = ++begin;
	if (it_next == vec.cend())
		return access(*it_cur).semantics().plterm();
	else {
		return ::term(EC_functor(functor.c_str(), 2),
			access(*it_cur).semantics().plterm(),
			to_ec_term(functor, vec, it_next)
		);
	}
}

template<>
EC_word to_ec_term(const string &functor, const vector<EC_word> &vec, vector<EC_word>::const_iterator begin);

template<class ListT>
EC_word to_ec_term(const string &functor, const ListT &vec)
{
	if (vec.size() < 1)
		throw Bug(string(__func__) + " must not be called on empty containers");
	return to_ec_term(functor, vec, vec.cbegin());
}



template<class ListT>
EC_word to_ec_list(const ListT &vec, typename ListT::const_iterator begin)
{
	if (begin == vec.cend())
		return ::nil();
	else {
		auto it_cur = begin;
		auto it_next = ++begin;
		return ::list(access(*it_cur).semantics().plterm(), to_ec_list(vec, it_next));
	}
}

template<>
EC_word to_ec_list(const vector<EC_word> &vec, typename vector<EC_word>::const_iterator begin);

template<class ListT>
EC_word to_ec_list(const ListT &vec)
{ return to_ec_list(vec, vec.cbegin()); }


EC_word make_ec_list(const std::vector<EC_word> &words);

EC_word copy_term(EC_word t);

string functor_name(EC_word t);

vector<unique_ptr<Expression>> plterm_args(EC_word t);


class ManagedTerm {
public:
	ManagedTerm(EC_word w);
	ManagedTerm(const ManagedTerm &);
	ManagedTerm();
	~ManagedTerm();

	bool initialized() const;

	operator EC_word () const;
	ManagedTerm &operator = (const EC_word &w);
	ManagedTerm &operator = (const ManagedTerm &);

private:
	void save(EC_word w);
	void clear();

	int index_;
	static int count_;
};


} // namespace gologpp

#endif // READYLOG_UTILITIES_H_
