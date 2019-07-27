#ifndef GOLOGPP_FLUENT_H_
#define GOLOGPP_FLUENT_H_

#include "gologpp.h"
#include "utilities.h"
#include "error.h"
#include "global.h"
#include "reference.h"
#include "constant.h"

namespace gologpp {


class InitialValue
: public virtual AbstractLanguageElement
, public NoScopeOwner
, public LanguageElement<InitialValue> {
public:
	InitialValue(const vector<Value *> &args, Value *value);
	InitialValue(const boost::optional<vector<Value *>> &args, Value *value);

	const vector<unique_ptr<Value>> &args() const;
	vector<unique_ptr<Value>> &args();
	void set_args(const vector<Value *> &args);

	const Value &value() const;
	Value &value();
	void set_value(Value *value);

	Fluent &fluent();
	const Fluent &fluent() const;
	void set_fluent(Fluent &f);

	virtual Scope &parent_scope() override;
	virtual const Scope &parent_scope() const override;

	virtual void attach_semantics(SemanticsFactory &implementor) override;

	virtual string to_string(const string &pfx) const override;

private:
	vector<unique_ptr<Value>> args_;
	unique_ptr<Value> value_;
	Fluent *fluent_;
};



class Fluent
: public Global
, public ScopeOwner
, public virtual AbstractLanguageElement
, public LanguageElement<Fluent>
{
public:
	Fluent(Scope *own_scope, const string &type_name, const string &name, const vector<shared_ptr<Variable>> &params);
	//Fluent(Scope &parent_scope, const string &name);

	virtual ~Fluent() override = default;

	const vector<unique_ptr<InitialValue>> &initially() const;

	void define(const vector<InitialValue *> &initial_values);
	void define(const boost::optional<vector<InitialValue *>> &initial_values);

	Reference<Fluent> *make_ref(const vector<Expression *> &params);
	virtual Expression *ref(const vector<Expression *> &params) override;

	virtual void attach_semantics(SemanticsFactory &implementor) override;

	virtual string to_string(const string &pfx) const override;

	virtual void compile(AExecutionContext &ctx) override;

private:
	vector<unique_ptr<InitialValue>> initial_values_;
};



} // namespace gologpp



#endif // GOLOGPP_FLUENT_H_
