#ifndef GOLOGPP_EXECUTION_H_
#define GOLOGPP_EXECUTION_H_

#include "utilities.h"
#include "Fluent.h"
#include "Action.h"
#include "Procedure.h"
#include "gologpp.h"

#include <memory>
#include <vector>
#include <unordered_map>
#include <tuple>

namespace gologpp {

class Situation;


class ExecutionContext {
public:
	template<class elem_t>
	using id_map_t = std::unordered_map<Identifier, std::shared_ptr<elem_t>>;

	ExecutionContext() = default;
	virtual ~ExecutionContext() = default;


	template<class ExprT, class... ArgTs>
	shared_ptr<Fluent<ExprT>> add_fluent(ArgTs &&... args)
	{ return add_global<Fluent<ExprT>>(fluents_, std::forward<ArgTs>(args)...); }

	shared_ptr<AbstractFluent> fluent(const string &name, arity_t arity)
	{ return get_global(fluents_, name, arity); }


	template<class... ArgTs>
	shared_ptr<Action> add_action(ArgTs &&... args)
	{ return add_global<Action>(actions_, std::forward<ArgTs>(args)...); }

	shared_ptr<Action> action(const string &name, arity_t arity)
	{ return get_global(actions_, name, arity); }


	template<class ExprT, class... ArgTs>
	shared_ptr<Function<ExprT>> add_function(ArgTs &&... args)
	{ return add_global<Function<ExprT>>(functions_, std::forward<ArgTs>(args)...); }

	shared_ptr<AbstractFunction> function(const string &name, arity_t arity)
	{ return get_global(functions_, name, arity); }


protected:
    id_map_t<AbstractFluent> fluents_;
    id_map_t<Action> actions_;
	id_map_t<AbstractFunction> functions_;

public:
	template<class RealT, class MappedT, class... ArgTs> inline
	shared_ptr<RealT> add_global(id_map_t<MappedT> &map, ArgTs &&... args)
	{
		shared_ptr<RealT> obj = std::make_shared<RealT>(std::forward<ArgTs>(args)...);
		auto result = map.insert( { Identifier(*obj), obj } );
		if (!result.second)
			throw std::runtime_error(obj->name() + "(" + to_string(obj->arity()) + ") already defined.");
		return obj;
	}


	template<class T> inline
	shared_ptr<T> get_global(id_map_t<T> &map, const string &name, arity_t arity)
	{
		auto it = map.find(Identifier(name, arity));
		if (it != map.end())
			return it->second;
		else
			return shared_ptr<T>();
	}
};


} // namespace gologpp

#endif
