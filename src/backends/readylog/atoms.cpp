#include "atoms.h"
#include <gologpp/atoms.h>

namespace gologpp {


Implementation<generic::Variable>::Implementation(const generic::Variable &var)
: variable_(var)
{}

void Implementation<generic::Variable>::init()
{ ec_var_ = ::newvar(); }

EC_word Implementation<generic::Variable>::term()
{ return ec_var_; }



Implementation<generic::AnyValue>::Implementation(const generic::AnyValue &val)
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


EC_word Implementation<generic::AnyValue>::term()
{ return boost::apply_visitor(wrap_word(), value_.value()); }


}
