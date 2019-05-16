#ifndef GOLOGPP_GLOBAL_H_
#define GOLOGPP_GLOBAL_H_

#include "gologpp.h"
#include "utilities.h"
#include "language.h"


namespace gologpp {


class Global
: public Identifier
, public std::enable_shared_from_this<Global>
, public virtual AbstractLanguageElement
{
protected:
	Global(const string &name, const vector<shared_ptr<Variable>> &args);

	template<class GologT>
	Reference<GologT> *make_ref_(const vector<Expression *> &args = {}) {
		static_assert(std::is_base_of<Identifier, GologT>::value,
		"Cannot reference a type that is not derived from Identifier");
		return new Reference<GologT>(
			std::dynamic_pointer_cast<GologT>(this->shared_from_this()),
			std::move(args)
		);
	}

public:
	virtual ~Global() override = default;

	vector<shared_ptr<Variable>> &args();
	const vector<shared_ptr<Variable>> &args() const;
	shared_ptr<Variable> argument(arity_t idx) const;
	void set_args(const vector<shared_ptr<Variable>> &args);

	Reference<Variable> *arg_ref(const string &name);

	virtual void compile(AExecutionContext &ctx) = 0;
	virtual Expression *ref(const vector<Expression *> &args = {}) = 0;

	virtual Scope &parent_scope() override;
	virtual const Scope &parent_scope() const override;

private:
	vector<shared_ptr<Variable>> args_;
};



} // namespace gologpp


#endif
