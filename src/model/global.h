#ifndef GOLOGPP_GLOBAL_H_
#define GOLOGPP_GLOBAL_H_

#include "utilities.h"
#include "language.h"
#include "variable.h"


namespace gologpp {



class Global
: public Identifier
, public std::enable_shared_from_this<Global>
, public virtual AbstractLanguageElement
{
protected:
	Global(const string &name, const vector<shared_ptr<AbstractVariable>> &args);

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

	vector<shared_ptr<AbstractVariable>> &args();
	const vector<shared_ptr<AbstractVariable>> &args() const;
	shared_ptr<AbstractVariable> argument(arity_t idx) const;
	void set_args(const vector<shared_ptr<AbstractVariable>> &args);

	template<class ExprT>
	Reference<Variable<ExprT>> *arg_ref(const string &name)
	{
		for (shared_ptr<AbstractVariable> &var : args_)
			if (var->name() == name)
				return std::dynamic_pointer_cast<Variable<ExprT>>(var)->ref();

		return nullptr;
	}

	virtual void compile(AExecutionContext &ctx) = 0;
	virtual Expression *ref(const vector<Expression *> &args = {}) = 0;

	virtual Scope &parent_scope() override;
	virtual const Scope &parent_scope() const override;

private:
	vector<shared_ptr<AbstractVariable>> args_;
};





} // namespace gologpp


#endif
