#include "utilities.h"

namespace gologpp {

Name::Name(const string &name)
: name_(name)
{}

Name::Name(Name &&other)
: name_(std::move(other.name_))
{}

Name::operator string () const
{ return name_; }

bool Name::operator == (const Name &other) const
{ return name() == other.name(); }

bool Name::operator != (const Name &other) const
{ return !(*this == other); }

size_t Name::hash() const
{ return std::hash<string>{}(name()); }

const string &Name::name() const
{ return name_; }



Identifier::Identifier(const string &name, arity_t arity)
: Name(name), arity_(arity)
{}

Identifier::Identifier(Identifier &&other)
: Name(std::move(other))
, arity_(std::move(other.arity()))
{}



arity_t Identifier::arity() const
{ return arity_; }

void Identifier::set_arity(arity_t a)
{ arity_ = a; }

bool Identifier::operator == (const Identifier &other) const
{ return name() == other.name() && arity() == other.arity(); }

bool Identifier::operator != (const Identifier &other) const
{ return !(*this == other); }

size_t Identifier::hash() const
{ return Name::hash() ^ (std::hash<gologpp::arity_t>{}(arity()) << 1); }

string Identifier::signature_str() const
{ return name() + "/" + std::to_string(arity()); }


#ifdef DEBUG_PARSER
const string indent("");
#else
const string indent("  ");
#endif


} // namespace gologpp
