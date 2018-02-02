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


class Fluent : public Atom, public Identifier, public std::enable_shared_from_this<Fluent>, public LanguageElement<Fluent> {
public:
	Fluent(const string &name, const vector<string> &args);
	Fluent(Fluent &&) = default;
	virtual ~Fluent() override = default;

	virtual tuple<> members() override;

private:
	shared_ptr<Scope> parent_scope_;
};


class Initially : public LanguageElement<Initially, Reference<Fluent>, AnyValue> {
public:
	Initially(Reference<Fluent> &&fluent, unique_ptr<AnyValue> &&value);
	virtual ~Initially() override = default;

	const Fluent &fluent() const;
	const AnyValue &initial_value() const;

	virtual tuple<Reference<Fluent> &, AnyValue &> members() override;

private:
	Reference<Fluent> fluent_;
	unique_ptr<AnyValue> value_;
};


} // namespace generic
} // namespace gologpp



#endif // GOLOGPP_FLUENT_H_
