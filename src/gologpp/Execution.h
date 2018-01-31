#ifndef GOLOGPP_EXECUTION_H_
#define GOLOGPP_EXECUTION_H_

#include "utilities.h"
#include "Fluent.h"
#include "Action.h"
#include "Procedure.h"

#include <memory>
#include <vector>
#include <unordered_map>
#include <tuple>

namespace gologpp {
namespace generic {

using namespace std;


class Transition;
class Situation;
class Fluent;
class Action;
class Procedure;


class History : public enable_shared_from_this<History> {
protected:
    vector<unique_ptr<Transition>> transitions_;
    shared_ptr<Situation> s0_;
};


template<class action_impl_t, class fluent_impl_t, class proc_impl_t>
struct ImplConfig {
};


template<class impl_config_t>
class ExecutionContext {
public:
	typedef typename impl_config_t::action_impl_t action_impl_t;
	typedef typename impl_config_t::fluent_impl_t fluent_impl_t;
	typedef typename impl_config_t::proc_impl_t proc_impl_t;

	ExecutionContext() = default;
	virtual ~ExecutionContext() = default;

	template<class T>
	shared_ptr<T> add_fluent(T &&f)
	{ return add_global(fluents_, std::move(f)); }

	shared_ptr<Fluent> fluent(const string &name, arity_t arity)
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


	void init()
	{
		for (auto entry : fluents_) {
			entry.second->init();
			entry.second->implementation().apply_to(*this);
		}
		for (auto entry : actions_) {
			entry.second->init();
			entry.second->implementation().apply_to(*this);
		}
		for (auto entry : procedures_) {
			entry.second->init();
			entry.second->implementation().apply_to(*this);
		}
	}

protected:

    shared_ptr<History> history_;
    unordered_map<NameWithArity, shared_ptr<Fluent>> fluents_;
    unordered_map<NameWithArity, shared_ptr<Action>> actions_;
    unordered_map<NameWithArity, shared_ptr<Procedure>> procedures_;

public:
	template<class T1, class T2> inline
	shared_ptr<T2> add_global(unordered_map<NameWithArity, shared_ptr<T1>> &map, T2 &&obj)
	{
		auto result = map.insert({NameWithArity(obj), dynamic_pointer_cast<T1>(make_shared<T2>(std::move(obj)))});
		// TODO
		// obj is moved from!
		/*if (!result.second)
			throw std::runtime_error(obj.name() + "(" + to_string(obj.arity()) + ") already defined.");*/
		return dynamic_pointer_cast<T2>(result.first->second);
	}


	template<class T> inline
	shared_ptr<T> get_global(unordered_map<NameWithArity, shared_ptr<T>> &map, const string &name, arity_t arity)
	{
		auto it = map.find(NameWithArity(name, arity));
		if (it != map.end())
			return it->second;
		else
			return shared_ptr<T>();
	}
};


} // namespace generic
} // namespace gologpp

#endif
