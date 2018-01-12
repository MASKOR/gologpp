#ifndef GOLOGPP_FLUENT_H_
#define GOLOGPP_FLUENT_H_

#include <string>
#include <memory>

#include "gologpp.h"
#include "atoms.h"

namespace gologpp {

using namespace std;

class Fluent : public Atom, public NameWithArity {
public:
	Fluent(const string &name, const vector<string> &args);
	virtual ~Fluent() = default;

private:
	vector<shared_ptr<Variable>> args;
};



} // namespace gologpp



#endif // GOLOGPP_FLUENT_H_
