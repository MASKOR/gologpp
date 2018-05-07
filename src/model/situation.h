#ifndef GOLOGPP_SITUATION_H
#define GOLOGPP_SITUATION_H

#include "atoms.h"
#include "fluent.h"
#include "utilities.h"

#include <unordered_map>

namespace gologpp {


class Situation : public LanguageElement<Situation>{
public:
	Situation();

	template<class ExprT>
	void add(shared_ptr<Fluent<ExprT>> fluent, unique_ptr<Constant<ExprT>> value)
	{
		fluent_values_.insert(
			std::dynamic_pointer_cast<AbstractFluent>(fluent),
			std::dynamic_pointer_cast<AbstractConstant>(value)
		);
	}


	template<class ExprT>
	Constant<ExprT> value(shared_ptr<Fluent<ExprT>> fluent)
	{
		fluent_values_.find(std::dynamic_pointer_cast<AbstractFluent>(fluent));
	}

protected:
	std::unordered_map<shared_ptr<AbstractFluent>, unique_ptr<AbstractConstant>, identifier_hash, identifier_equals> fluent_values_;
};


} // namespace gologpp

#endif // GOLOGPP_SITUATION_H