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
#include <mutex>
#include <queue>
#include <condition_variable>

namespace gologpp {

class Situation;
class History;


class HistoryImplementation : public AbstractImplementation {
public:
	HistoryImplementation(History &history);
	virtual ~HistoryImplementation();

	virtual void append_exog(ExogTransition &&) = 0;

protected:
	History &history_;
};


class History : public LanguageElement<History> {
public:
	History();

	DEFINE_IMPLEMENT

	HistoryImplementation &abstract_impl()
	{ return dynamic_cast<HistoryImplementation &>(*impl_); }

};


template<class ImplementorT>
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

	shared_ptr<AbstractAction> action(const string &name, arity_t arity)
	{ return get_global(actions_, name, arity); }


	template<class ExprT, class... ArgTs>
	shared_ptr<Function<ExprT>> add_function(ArgTs &&... args)
	{ return add_global<Function<ExprT>>(functions_, std::forward<ArgTs>(args)...); }

	shared_ptr<AbstractFunction> function(const string &name, arity_t arity)
	{ return get_global(functions_, name, arity); }

	virtual bool final(Block &program, History &h) = 0;
	virtual bool trans(Block &program, History &h) = 0;

	virtual void compile(const Block &program) = 0;
	virtual void compile(const AbstractFluent &fluent) = 0;
	virtual void compile(const AbstractAction &action) = 0;
	virtual void compile(const AbstractFunction &function) = 0;

	ExogTransition exog_queue_pop()
	{
		std::lock_guard<std::mutex> { exog_mutex_ };
		ExogTransition rv = std::move(exog_queue_.front());
		exog_queue_.pop();
		return rv;
	}

	ExogTransition exog_queue_poll()
	{
		std::unique_lock<std::mutex> queue_empty_lock { queue_empty_mutex_ };
		queue_empty_condition_.wait(queue_empty_lock, [&] { return !exog_queue_.empty(); });
		return exog_queue_pop();
	}

	void exog_queue_push(ExogTransition &&exog)
	{
		std::lock_guard<std::mutex> { exog_mutex_ };
		exog_queue_.push(std::move(exog));
		{
			std::lock_guard<std::mutex> { queue_empty_mutex_ };
			queue_empty_condition_.notify_one();
		}
	}

	History run(Block &&program) {
		ImplementorT implementor;

		History history;
		history.implement(implementor);

		for (id_map_t<AbstractFluent>::value_type &entry : fluents_) {
			entry.second->implement(implementor);
			compile(*entry.second);
		}
		for (id_map_t<AbstractAction>::value_type &entry : actions_) {
			entry.second->implement(implementor);
			compile(*entry.second);
		}
		for (id_map_t<AbstractFunction>::value_type &entry : functions_) {
			entry.second->implement(implementor);
			compile(*entry.second);
		}

		program.implement(implementor);

		while (!final(program, history)) {
			while (!exog_queue_.empty()) {
				ExogTransition exog = exog_queue_pop();
				exog.implement(implementor);
				history.abstract_impl().append_exog(std::move(exog));
			}
			if (!trans(program, history)) {
				ExogTransition exog = exog_queue_poll();
				exog.implement(implementor);
				history.abstract_impl().append_exog(std::move(exog));
			}
		}
		return history;
	}


protected:
    id_map_t<AbstractFluent> fluents_;
    id_map_t<AbstractAction> actions_;
	id_map_t<AbstractFunction> functions_;

	std::mutex exog_mutex_;
	std::condition_variable queue_empty_condition_;
	std::mutex queue_empty_mutex_;
	std::queue<ExogTransition> exog_queue_;

public:
	template<class RealT, class MappedT, class... ArgTs> inline
	shared_ptr<RealT> add_global(id_map_t<MappedT> &map, ArgTs &&... args)
	{
		shared_ptr<RealT> obj = std::make_shared<RealT>(std::forward<ArgTs>(args)...);
		auto result = map.insert( { Identifier(*obj), obj } );
		if (!result.second)
			throw std::runtime_error(obj->name() + "(" + std::to_string(obj->arity()) + ") already defined.");
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
