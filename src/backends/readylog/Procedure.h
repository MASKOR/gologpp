#ifndef READYLOG_PROCEDURE_H_
#define READYLOG_PROCEDURE_H_

#include <gologpp/Implementation.h>
#include <eclipseclass.h>

#include "Implementation.h"

namespace gologpp {

namespace generic {
class Procedure;
}

template<>
class Implementation<generic::Procedure> : public ReadylogExpression {
public:
	Implementation(const generic::Procedure &proc);

	virtual EC_word term() override;

private:
	const generic::Procedure &procedure_;
};


template<>
class Implementation<generic::Block> : public ReadylogExpression {
public:
	Implementation(const generic::Block &);
	virtual EC_word term() override;

private:
	const generic::Block &block_;
};


template<>
class Implementation<generic::Choose> : public ReadylogExpression {
public:
	Implementation(const generic::Choose &);
	virtual EC_word term() override;

private:
	const generic::Choose &choose_;
};


template<>
class Implementation<generic::Conditional> : public ReadylogExpression {
public:
	Implementation(const generic::Conditional &);
	virtual EC_word term() override;

private:
	const generic::Conditional &conditional_;
};


template<>
class Implementation<generic::Assignment> : public ReadylogExpression {
public:
	Implementation(const generic::Assignment &);
	virtual EC_word term() override;

private:
	const generic::Assignment &assignment_;
};


template<>
class Implementation<generic::Pick> : public ReadylogExpression {
public:
	Implementation(const generic::Pick &);
	virtual EC_word term() override;
};


template<>
class Implementation<generic::Search> : public ReadylogExpression {
public:
	Implementation(const generic::Search &);
	virtual EC_word term() override;
};


template<>
class Implementation<generic::Test> : public ReadylogExpression {
public:
	Implementation(const generic::Test &);
	virtual EC_word term() override;
};


template<>
class Implementation<generic::While> : public ReadylogExpression {
public:
	Implementation(const generic::While &);
	virtual EC_word term() override;
};


} // namespace gologpp


#endif // READYLOG_PROCEDURE_H_
