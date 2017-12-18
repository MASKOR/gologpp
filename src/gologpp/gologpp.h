#ifndef GOLOGPP_H_
#define GOLOGPP_H_

#include <string>
#include <functional>
#include <memory>

namespace gologpp {

using namespace std;

typedef unsigned char arity_t;


class NameWithArity {
public:
	NameWithArity(const string &name, arity_t arity)
	: name_(name), arity_(arity)
	{}

	virtual ~NameWithArity()
	{}

	arity_t arity() const
	{ return arity_; }

	const string &name() const
	{ return name_; }

	virtual bool operator == (const NameWithArity &other) const
	{ return name() == other.name() && arity() == other.arity(); }

	virtual size_t hash() const
	{ return std::hash<string>{}(name()) ^ (std::hash<gologpp::arity_t>{}(arity()) << 1); }

private:
	const string name_;
	const arity_t arity_;
};

} // namespace gologpp


namespace std {

template<>
struct hash<gologpp::NameWithArity> {
	size_t operator () (const gologpp::NameWithArity &o) const
	{ return o.hash(); }
};


} // namespace std


#endif // GOLOGPP_H_