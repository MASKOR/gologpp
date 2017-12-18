#ifndef FLUENT_H
#define FLUENT_H


#include <string>
#include <memory>

#include "gologpp.h"
#include "Formula.h"

namespace gologpp {

using namespace std;

class Fluent : public Atom, public NameWithArity {
public:
	Fluent(const string &name, arity_t arity);
	Fluent(const Fluent &) = default;
	Fluent(Fluent &&) = default;
	virtual ~Fluent() = default;
};


} // namespace gologpp



#endif // FLUENT_H
