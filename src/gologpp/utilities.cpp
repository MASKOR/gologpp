#include "utilities.h"
#include "atoms.h"

namespace gologpp {
namespace generic {

Name::Name(const string &name)
: name_(name)
{}

Name::Name(Name &&other)
: name_(std::move(other.name_))
{}

const string &Name::name() const
{ return name_; }

bool Name::operator == (const Name &other) const
{ return name() == other.name(); }

size_t Name::hash() const
{ return std::hash<string>{}(name()); }


Identifier::Identifier(const string &name, arity_t arity)
: Name(name), arity_(arity)
{}

Identifier::Identifier(Identifier &&other)
: Name(std::move(other))
, arity_(std::move(other.arity()))
{}



arity_t Identifier::arity() const
{ return arity_; }

bool Identifier::operator == (const Identifier &other) const
{ return name() == other.name() && arity() == other.arity(); }

size_t Identifier::hash() const
{ return Name::hash() ^ (std::hash<gologpp::arity_t>{}(arity()) << 1); }



} // namespace generic
} // namespace gologpp
