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


	template<class T>
	shared_ptr<T> add_fluent(T &&f)
	{ return add_global(fluents_, std::move(f)); }

	shared_ptr<AbstractFluent> fluent(const string &name, arity_t arity)
	{ return get_global(fluents_, name, arity); }


	template<class T>
	shared_ptr<T> add_action(T &&f)
	{ return add_global(actions_, std::move(f)); }

	shared_ptr<Action> action(const string &name, arity_t arity)
	{ return get_global(actions_, name, arity); }


	template<class T>
	shared_ptr<T> add_procedure(T &&f)
	{ return add_global(procedures_, std::move(f)); }

	shared_ptr<Procedure> procedure(const string &name, arity_t arity)
	{ return get_global(procedures_, name, arity); }


protected:
    id_map_t<AbstractFluent> fluents_;
    id_map_t<Action> actions_;
    id_map_t<Procedure> procedures_;

public:
	template<class T1, class T2> inline
	shared_ptr<T2> add_global(unordered_map<Identifier, shared_ptr<T1>> &map, T2 &&obj)
	{
		auto result = map.insert({Identifier(obj), dynamic_pointer_cast<T1>(make_shared<T2>(std::move(obj)))});
		// TODO
		// obj is moved from!
		/*if (!result.second)
			throw std::runtime_error(obj.name() + "(" + to_string(obj.arity()) + ") already defined.");*/
		return dynamic_pointer_cast<T2>(result.first->second);
	}


	template<class T> inline
	shared_ptr<T> get_global(unordered_map<Identifier, shared_ptr<T>> &map, const string &name, arity_t arity)
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
