#include "atoms.h"

namespace gologpp {


Implementation<Variable>::Implementation(const Variable &var)
: variable_(var)
{}

void Implementation<Variable>::init()
{ ec_var_ = ::newvar(); }

EC_word Implementation<Variable>::term()
{ return ec_var_; }



Implementation<AnyValue>::Implementation(const AnyValue &val)
: value_(val)
{}


struct wrap_word {
template<class T>
	EC_word operator () (const T &v) const
	{ return ::EC_word(v); }
};

template<>
EC_word wrap_word::operator () (const string &v) const
{ return ::EC_word(v.c_str()); }

template<>
EC_word wrap_word::operator () (const bool &v) const
{ return v ? EC_atom("true") : EC_atom("fail"); }


EC_word Implementation<AnyValue>::term()
{ return boost::apply_visitor(wrap_word(), value_.value()); }


}
