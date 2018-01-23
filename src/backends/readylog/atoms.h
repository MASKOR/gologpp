#ifndef READYLOG_ATOMS_H_
#define READYLOG_ATOMS_H_

#include <gologpp/atoms.h>
#include <gologpp/Translation.h>
#include <eclipseclass.h>

namespace gologpp {
namespace readylog {


class Variable : public Translatable<generic::Variable, EC_word> {
public:
	Variable(generic::Variable &&var);

private:
	virtual void init(EC_word &cache) override;
	EC_ref value;
};


class AnyValue : public Translatable<generic::AnyValue, EC_word> {
};


template<typename DataT>
class Value : public Translatable<generic::Value<DataT>, EC_word> {
public:
	using Translatable<generic::Value<DataT>, EC_word>::Translatable;

private:
	virtual void init(EC_word &cache) override {
		cache = EC_word(generic::Value<DataT>::data());
	}
};


} // namespace readylog
} // namespace gologpp



#endif