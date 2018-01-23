#ifndef GOLOGPP_FLUENT_H_
#define GOLOGPP_FLUENT_H_

#include <string>
#include <memory>

#include "gologpp.h"
#include "atoms.h"
#include "utilities.h"
#include "Reference.h"

namespace gologpp {
namespace generic {

using namespace std;

class Fluent : public Atom, public NameWithArity, public enable_shared_from_this<Fluent>, public LanguageElement<Fluent> {
public:
	Fluent(const string &name, const vector<string> &args);
	Fluent(Fluent &&) = default;
	virtual ~Fluent() = default;

private:
	shared_ptr<Scope> parent_scope_;
};


class Initially : public LanguageElement<Initially> {
public:
	Initially(FluentReference &&fluent, unique_ptr<AnyValue> &&value);
	virtual ~Initially() = default;

	const Fluent &fluent() const;
	const AnyValue &initial_value() const;

private:
	FluentReference fluent_;
	unique_ptr<AnyValue> value_;
};


} // namespace generic
} // namespace gologpp



#endif // GOLOGPP_FLUENT_H_
