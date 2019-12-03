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

#ifndef GOLOGPP_UTILITIES_H_
#define GOLOGPP_UTILITIES_H_

#include "gologpp.h"
#include <string>
#include <type_traits>


namespace gologpp {


class Name {
public:
    Name(const string &name);
    Name(Name &&other);
    Name(const Name &other) = default;
    virtual ~Name() = default;
    
    explicit operator string () const;
    const string &name() const;
    virtual bool operator == (const Name &other) const;
    bool operator != (const Name &other) const;
    virtual size_t hash() const;
    
protected:
    string name_;
};



class Identifier : public Name {
public:
    Identifier(const string &name, arity_t arity);
    Identifier(Identifier &&other);
    Identifier(const Identifier &other) = default;

	template<class ArgsT>
    Identifier(const string &name, const vector<ArgsT> &args)
    : Identifier(name, static_cast<arity_t>(args.size()))
    {}

    virtual ~Identifier() override = default;

    arity_t arity() const;
    void set_arity(arity_t);

    virtual bool operator == (const Identifier &other) const;
    bool operator != (const Identifier &other) const;
    virtual size_t hash() const override;

	string signature_str() const;

private:
    arity_t arity_;
};



struct identifier_hash {
	template<class IdPtrT>
	std::size_t operator () (const IdPtrT &p) const {
		return std::dynamic_pointer_cast<Identifier>(p)->hash();
	}
};


struct identifier_equals {
	template<class IdPtrT>
	bool operator () (const IdPtrT &lhs, const IdPtrT &rhs) const {
		return *std::dynamic_pointer_cast<Identifier>(lhs) == *std::dynamic_pointer_cast<Identifier>(rhs);
	}
};



template<class CharT, class TraitsT, class GologT>
typename std::enable_if<
	std::is_base_of<AbstractLanguageElement, GologT>::value
	|| std::is_base_of<Expression, GologT>::value,
	std::basic_ostream<CharT, TraitsT>
>::type
&operator << (
	std::basic_ostream<CharT, TraitsT> &stream,
	const GologT &obj
)
{ return stream << obj.to_string(""); }


template<class CharT, class TraitsT, class GologT>
typename std::enable_if<
	std::is_base_of<AbstractLanguageElement, GologT>::value
	|| std::is_base_of<Expression, GologT>::value,
	std::basic_ostream<CharT, TraitsT>
>::type
&operator << (
	std::basic_ostream<CharT, TraitsT> &stream,
	const GologT *obj
)
{ return stream << (obj ? obj->to_string("") : "nullptr"); }


template<class CharT, class TraitsT, class GologT>
typename std::enable_if<
	std::is_base_of<AbstractLanguageElement, GologT>::value
	|| std::is_base_of<Expression, GologT>::value,
	std::basic_ostream<CharT, TraitsT>
>::type
&operator << (
	std::basic_ostream<CharT, TraitsT> &stream,
	const shared_ptr<GologT> &obj
)
{ return stream << (obj ? obj->to_string("") : "nullptr"); }


template<class CharT, class TraitsT, class ElemT>
std::basic_ostream<CharT, TraitsT>
&operator << (
	std::basic_ostream<CharT, TraitsT> &stream,
	const vector<ElemT> &v
)
{
	stream << "[";
	typename vector<ElemT>::const_iterator it = v.begin();
	for (const ElemT &e : v) {
		stream << e;
		if (++it < v.end())
			stream << ", ";
	}
	return stream << "]";
}



extern const string indent;

#ifdef DEBUG_PARSER
#define linesep " "
#else
#define linesep "\n"
#endif



template<class T>
string to_string(const unique_ptr<T> &o, const string &pfx)
{ return o->to_string(pfx); }

template<class T>
string to_string(const shared_ptr<T> &o, const string &pfx)
{ return o->to_string(pfx); }

template<class T>
string to_string(const T &o, const string &pfx)
{ return o.to_string(pfx); }



template<class ListT>
string concat_list(const ListT &l, const string sep, const string &pfx = "")
{
	string rv;
	for (const auto &elem : l)
		rv += to_string(elem, pfx) + sep;
	return rv.length() ? rv.substr(0, rv.length() - sep.length()) : rv;
}


template<class T, class T1, class... Ts>
struct is_member {
	static constexpr bool value()
	{ return std::is_same<T, T1>::value || (sizeof...(Ts) && is_member<T, Ts...>::value()); }
};





} // namespace gologpp



namespace std {

template<>
struct hash<gologpp::Identifier> {
    size_t operator () (const gologpp::Identifier &o) const
    { return o.hash(); }
};

template<>
struct hash<gologpp::Name> {
	size_t operator () (const gologpp::Name &o) const
	{ return o.hash(); }
};

} // namespace std

#endif // GOLOGPP_UTILITIES_H_
