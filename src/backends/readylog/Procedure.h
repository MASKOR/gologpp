#ifndef READYLOG_PROCEDURE_H_
#define READYLOG_PROCEDURE_H_

#include <gologpp/Implementation.h>
#include <eclipseclass.h>

#include "Implementation.h"

namespace gologpp {

template<>
class Implementation<Procedure> : public ReadylogImplementation {
public:
	Implementation(const Procedure &proc);

	virtual EC_word term() override;
	EC_word definition();

private:
	const Procedure &procedure_;
};


template<>
class Implementation<Block> : public ReadylogImplementation {
public:
	Implementation(const Block &);
	virtual EC_word term() override;

private:
	const Block &block_;
};


template<>
class Implementation<Choose> : public ReadylogImplementation {
public:
	Implementation(const Choose &);
	virtual EC_word term() override;

private:
	const Choose &choose_;
};


template<>
class Implementation<Conditional> : public ReadylogImplementation {
public:
	Implementation(const Conditional &);
	virtual EC_word term() override;

private:
	const Conditional &conditional_;
};


template<>
class Implementation<Assignment> : public ReadylogImplementation {
public:
	Implementation(const Assignment &);
	virtual EC_word term() override;

private:
	const Assignment &assignment_;
};


template<>
class Implementation<Pick> : public ReadylogImplementation {
public:
	Implementation(const Pick &);
	virtual EC_word term() override;
};


template<>
class Implementation<Search> : public ReadylogImplementation {
public:
	Implementation(const Search &);
	virtual EC_word term() override;
};


template<>
class Implementation<Test> : public ReadylogImplementation {
public:
	Implementation(const Test &);
	virtual EC_word term() override;
};


template<>
class Implementation<While> : public ReadylogImplementation {
public:
	Implementation(const While &);
	virtual EC_word term() override;
};


} // namespace gologpp


#endif // READYLOG_PROCEDURE_H_
