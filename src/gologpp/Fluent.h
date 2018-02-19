#ifndef GOLOGPP_FLUENT_H_
#define GOLOGPP_FLUENT_H_

#include <string>
#include <memory>

#include "gologpp.h"
#include "atoms.h"
#include "utilities.h"
#include "Reference.h"
#include "Scope.h"

namespace gologpp {


class Fluent : public Atom, public Identifier, public std::enable_shared_from_this<Fluent>, public LanguageElement<Fluent> {
public:
	Fluent(const string &name, const vector<string> &args);
	Fluent(Fluent &&);
	//virtual ~Fluent() override = default;

	const vector<string> &args() const;
	DEFINE_IMPLEMENT_WITH_MEMBERS(scope_)

	Scope &scope();
	const Scope &scope() const;

private:
	vector<string> args_;
	Scope scope_;
};


class Initially : public LanguageElement<Initially> {
public:
	Initially(Reference<Fluent> &&fluent, unique_ptr<AnyValue> &&value);
	//virtual ~Initially() override = default;

	const Fluent &fluent() const;
	const AnyValue &initial_value() const;

private:
	Reference<Fluent> fluent_;
	unique_ptr<AnyValue> value_;
};


} // namespace gologpp



#endif // GOLOGPP_FLUENT_H_
