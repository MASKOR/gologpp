#ifndef GOLOGPP_EXECUTION_H_
#define GOLOGPP_EXECUTION_H_

#include "gologpp.h"

#include <memory>
#include <vector>
#include <unordered_map>
#include <tuple>

namespace gologpp {

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


class ExecutionContext {
public:
	ExecutionContext() = default;

	shared_ptr<Fluent> fluent(const string &name, arity_t arity);
	shared_ptr<Action> action(const string &name, arity_t arity);
	shared_ptr<Procedure> procedure(const string &name, arity_t arity);

protected:
    shared_ptr<History> history_;
    unordered_map<NameWithArity, shared_ptr<Fluent>> fluents_;
    unordered_map<NameWithArity, shared_ptr<Action>> actions_;
    unordered_map<NameWithArity, shared_ptr<Procedure>> procedures_;

private:
	template<class T>
	shared_ptr<T> get_global(unordered_map<NameWithArity, shared_ptr<T>> map, const string &name, arity_t arity)
	{
		auto it = map.find(NameWithArity(name, arity));
		if (it != map.end())
			return it->second;
		else
			return shared_ptr<T>();
	}
};


class LanguageElement {
public:
	virtual ~LanguageElement();

	virtual void define(ExecutionContext &context) = 0;
};


}

#endif