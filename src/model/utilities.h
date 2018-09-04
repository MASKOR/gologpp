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
    
    const string &name() const;
    virtual bool operator == (const Name &other) const;
    virtual size_t hash() const;
    
private:
    const string name_;
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

    virtual bool operator == (const Identifier &other) const;
    virtual size_t hash() const override;

private:
    const arity_t arity_;
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
) { return stream << obj.to_string(""); }



extern const string indent;

#define linesep "\n"



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


} // namespace gologpp



namespace std {

template<>
struct hash<gologpp::Identifier> {
    size_t operator () (const gologpp::Identifier &o) const
    { return o.hash(); }
};

} // namespace std

#endif // GOLOGPP_UTILITIES_H_
